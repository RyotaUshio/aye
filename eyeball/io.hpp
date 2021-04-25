#pragma once

#include <eyeball/core.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

namespace eyeball {

  Image normalize(const Image& image, Image::dtype upper_bound=255.0) {
    auto max = numpy::max(image);
    auto min = numpy::min(image);
    auto out = image.copy();
    out -= min;
    out *= upper_bound / (max - min);
    return out;
  }

  template <class... Args>
  void savetxt(const char* fname, const Image& image, Args... args) {
    numpy::savetxt(fname, normalize(image),
		   "%.18f", // fmt
		   " ", // delimiter
		   " ", // newline
		   python::str(image.shape(0)) + " " + python::str(image.shape(1)) + "\n", // header
		   "" // footer
		   );
  }

  template <class... Args>
  Image loadtxt(const char* fname, Args... args) {
    return numpy::loadtxt(fname, args...);
  }

  template <class... Args>
  Image imread(int argc, const char** argv, int argc_required, Args... args) {
    if (argc < argc_required)
      throw std::runtime_error(python::str(argc_required) +
			       " arguments are required, got " +
			       python::str(argc));
    return loadtxt(argv[1], args...);
  }

}
