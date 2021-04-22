#pragma once

#include <vision/core.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

namespace vision {

  // Image imread(const std::string& filename) {}

  Image normalize(const Image& image) {
    auto max = numpy::max(image);
    auto min = numpy::min(image);
    auto out = image.copy();
    out -= min;
    out *= 256.0 / (max - min);
    return out;
  }

  template <class... Args>
  void savetxt(const char* fname, const Image& image, Args... args) {
    numpy::savetxt(fname, normalize(image), args...);
  }
}
