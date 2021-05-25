#pragma once

#include <numpy/numpy.hpp>
#include <vector>

namespace aye {
  using Image = numpy::ndarray<numpy::float_>;
  using Kernel = numpy::ndarray<numpy::float_>;
  using CImage = numpy::ndarray<numpy::complex_>;
  using CKernel = numpy::ndarray<numpy::complex_>;
  using Images = std::vector<Image>;
}

namespace np = numpy;

