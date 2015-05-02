//Copyright (c) 2015 Zachary Kann
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

// ---
// Author: Zachary Kann

// A set of functions for splitting, trimming, or otherwise
// manipulating strings.

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

#ifndef _Z_STRING_HPP_
#define _Z_STRING_HPP_

// Parses a string into smaller substrings by splitting at delim.
extern std::vector<std::string>& Split(const std::string& s, char delim,
                                       std::vector<std::string> &elems);

// A wrapper for split used if the element vector does not exist.
extern std::vector<std::string> Split(const std::string& s, char delim);

// Trims whitespace from the left side of a string
static inline std::string& LTrim(std::string& s) {
  s.erase(s.begin(), find_if(s.begin(), s.end(),
          std::not1(std::ptr_fun<int, int>(isspace))));
  return s;
}

// Trims whitespace from the right side of a string
static inline std::string& RTrim(std::string& s) {
  s.erase(find_if(s.rbegin(), s.rend(),
          std::not1(std::ptr_fun<int, int>(isspace))).base(), s.end());
  return s;
}

// Trims whitespace from both sides of a string
static inline std::string& Trim(std::string& s) { return LTrim(RTrim(s)); }

#endif
