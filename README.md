This program is for the calculation of potential of mean force (PMF) from umbrella-sampling simulations 
using the weighted histogram analysis method (WHAM) as outlined in the work of Benoît Roux 
(Roux B. Comput Phys Comm. 1995;91:275–282.).

THIS PROGRAM HAS NOT BEEN THOROUGHLY TESTED!!!

Current limitations of the program include:
* Only usable with data from the GROMACS simulation package.
* Only usable for one-dimensional PMFs.
* Input data must be in the pullx.xvg format from GROMACS, use of pullf.xvg is not yet implemented.
* Several important parameters are hard-coded, including temperature and histogram range.

The primary reasons this program exists are:
* I needed to thoroughly understand the implementation of WHAM.
* The wham program by Grossfield requires a complicated metadata file. Much of the data in that file 
  (e.g., centers of the umbrella potentials and the temperature) can easily be read directly from
  various input or output files of the simulations themselves.
