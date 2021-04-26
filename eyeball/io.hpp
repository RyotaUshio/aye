#pragma once

#include <eyeball/core.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

namespace eyeball {

  template <class Array>
  Array normalize(const Array& image, typename Array::dtype upper_bound=255) {
    auto max = numpy::max(image);
    auto min = numpy::min(image);
    auto out = image.copy();
    out -= min;
    out *= upper_bound / (max - min);
    return out;
  }

  template <class Array, class... Args>
  void savetxt(const char* fname, const Array& image, Args... args) {
    numpy::savetxt(fname, normalize(image),
		   //"%.18f", // fmt
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

  template <class Array, class... Args>
  void imshow(const Array& image) {
    std::string filename = "__eyeball_imshow__";
    savetxt<Array>(filename.c_str(), image);
    std::system(("./io imshow " + filename).c_str());
    std::system(("rm " + filename).c_str());
  }

}
