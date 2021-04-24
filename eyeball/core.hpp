#pragma once

#include <numpy/numpy.hpp>

namespace eyeball {
  using Image = numpy::ndarray<double>;
  using Kernel = numpy::ndarray<double>;
}

namespace np = numpy;
namespace eye = eyeball;
