#pragma once

#include <numpy/numpy.hpp>

namespace vision {
  using Image = numpy::ndarray<double>;
  using Kernel = numpy::ndarray<double>;
}
