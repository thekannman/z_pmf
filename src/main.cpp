#include <iomanip>
#include <cmath>
#include <fstream>
#include "z_constants.hpp"
#include "z_string.hpp"
#include "boost/filesystem.hpp"

namespace fs = ::boost::filesystem;
// Units are nm, ps.

void get_filetype(const fs::path& root, const std::string& ext,
                  std::vector<fs::path>& ret);
bool is_converged(std::vector<double> new_values,
                  std::vector<double> old_values, double TOL);

int main (int argc, char *argv[]) {
  std::vector<fs::path> xvg_files;
  get_filetype(".", ".xvg", xvg_files);
  sort(xvg_files.begin(),xvg_files.end());
  int NUM_WINDOWS = xvg_files.size();
  double HIST_MAX = 4.0, HIST_MIN = 1.50, BIN_WIDTH, TOL = 1e-06;
  int MAX_ITER = 1000000;
  double T = 235.0;
  double RT = R_GAS*T;
  int  NUM_BINS = 100;
  BIN_WIDTH = (HIST_MAX - HIST_MIN) / (double) NUM_BINS;
  std::vector<std::vector<int> > hist(NUM_BINS, std::vector<int>(NUM_WINDOWS));
  std::vector<double> window_center;
  std::vector<int> step_counts;

  int i=0;
  for (std::vector<fs::path>::iterator i_xvg=xvg_files.begin();
       i_xvg!=xvg_files.end(); i_xvg++, i++) {
    std::ifstream xvg_file;
    xvg_file.open((*i_xvg).c_str());
    std::string line;
    std::string subline;
    std::vector<std::string> split_line;

    do {
      getline(xvg_file, line);
    } while (line[0] == '#' || line[0] == '@');
    split_line = Split(line, '\t');
    double center = atof(split_line[1].c_str());
    if(center < HIST_MIN || center > HIST_MAX) {
      std::cout << (*i_xvg).c_str() << " " << HIST_MIN << " " << center <<
                   " " << HIST_MAX << std::endl;
      assert(false && "Window center outside of histogram range.\n");
    }
    window_center.push_back(center);
    int step_count = 1;
    while (getline(xvg_file, line)) {
      std::vector<std::string> split_line = Split(line, '\t');
      double x_i = atof(split_line[1].c_str());
      if(x_i <= HIST_MIN || x_i >= HIST_MAX) continue;
      step_count++;
      int which_bin = (x_i - HIST_MIN)/BIN_WIDTH;
      hist[which_bin][i]++;
    }
    step_counts.push_back(step_count);
    xvg_file.close();
  }
  std::ofstream hist_file;
  hist_file.open("hists.txt");
  for (std::vector<std::vector<int> >::iterator i_hist=hist.begin();
       i_hist != hist.end(); i_hist++) {
    hist_file << std::setw(5) <<
                 HIST_MIN + ((i_hist-hist.begin())+0.5)*BIN_WIDTH;
    for (std::vector<int>::iterator i_window=(*i_hist).begin();
         i_window != (*i_hist).end(); i_window++) {
      hist_file << " " << std::setw(7) << (*i_window);
    }
    hist_file << std::endl;
  }
  std::cout << "Data reading finished. Starting WHAM." << std::endl;
  std::vector<double> F(NUM_WINDOWS);
  std::vector<double> F_old(NUM_WINDOWS);
  std::vector<double> prob(NUM_BINS);
  double force_constant = 10000.0;
  int iteration = 0;
  do {
    F_old = F;
    std::fill(F.begin(), F.end(), 0.0);
    iteration++;
    std::cout << "Iteration: " << iteration << std::endl;
    std::vector<double>::iterator i_prob=prob.begin();
    std::vector<std::vector<int> >::iterator i_hist= hist.begin();
    for (int i_bin = 0; i_bin < NUM_BINS; i_bin++, i_hist++, i_prob++) {
      double bin_center = HIST_MIN + ((double)i_bin + 0.5)*BIN_WIDTH;
      double numer = 0.0, denom = 0.0;
      std::vector<int>::iterator i_steps = step_counts.begin();
      std::vector<int>::iterator i_window = (*i_hist).begin();
      for (std::vector<double>::iterator i_center = window_center.begin(),
               i_F = F_old.begin();
           i_center != window_center.end();
           i_window++, i_F++, i_center++, i_steps++) {
        numer += (double)(*i_window);
        double bias = 0.5*force_constant*std::pow(bin_center-(*i_center), 2.0);
        denom += (*i_steps) * exp((-(bias-(*i_F))) / RT);
      }
      assert(denom == denom);
      (*i_prob) = numer/denom;
      for (std::vector<double>::iterator i_center = window_center.begin(),
               i_F = F.begin();
           i_F != F.end(); i_center++, i_F++) {
        double window_center = (*i_center);
        double bias =
            0.5*force_constant*std::pow(bin_center-window_center, 2.0);
        (*i_F) += (*i_prob) * exp(-bias/RT);
      }
    }
    for (std::vector<double>::iterator i_F = F.begin(); i_F != F.end(); i_F++)
      (*i_F) = -RT * log((*i_F));
    for (std::vector<double>::iterator i_F = F.end()-1; i_F >= F.begin(); i_F--)
      (*i_F) -= (*(F.begin()));
  } while(!is_converged(F, F_old, TOL) && iteration < MAX_ITER);
  double prob_sum = 0.0;
  for (i=0; i < NUM_BINS; i++)
    prob_sum += prob[i];
  for (i=0; i < NUM_BINS; i++)
    prob[i] /= prob_sum;;
  std::ofstream pmf_file;
  pmf_file.open("profile.txt");
  for (std::vector<double>::iterator i_prob = prob.begin();
       i_prob != prob.end(); i_prob++) {
    pmf_file << HIST_MIN + ((i_prob-prob.begin())+0.5)*BIN_WIDTH << " " <<
                -RT*log((*i_prob)) << std::endl;
  }
}

void get_filetype(const fs::path& root, const std::string& ext,
                  std::vector<fs::path>& ret) {
    //http://stackoverflow.com/questions/11140483/how-to-get-list-of-files-with-a-specific-extension-in-a-given-folder
  if(!fs::exists(root) || !fs::is_directory(root)) return;
    fs::directory_iterator it(root);
    fs::directory_iterator endit;
  while(it != endit) {
    if(fs::is_regular_file(*it) && it->path().extension() == ext)
      ret.push_back(it->path().filename());
    ++it;
  }
}

bool is_converged(std::vector<double> new_values,
                  std::vector<double> old_values, double TOL) {
  bool converged = true;
  double max_change = 0.0;
  int which;
  for (std::vector<double>::iterator i_new = new_values.begin(),
           i_old=old_values.begin();
       i_new!=new_values.end(); i_new++, i_old++) {
    double change = abs((*i_new)-(*i_old));
    if (change > max_change) {
      max_change = change;
      which = (i_new-new_values.begin());
    }
    if (change > TOL) converged = false;
  }
  std::cout << "Largest change is: " << max_change << " for bin: " << which <<
               std::endl;
  return converged;
}
