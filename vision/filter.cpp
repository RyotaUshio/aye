#include <filter.hpp>

namespace vision {

  void pad_zero(const Image& image, std::size_t pad_size, Image& out) {
    auto hi = image.shape(0); // height of the input image
    auto wid = image.shape(1); // width of the input image
    auto hi_out = out.shape(0); // height of the output image
    auto wid_out = out.shape(1); // width of the output image

    if ((hi + pad_size *2 != hi_out) or (wid + pad_size * 2 != wid_out))
      throw std::invalid_argument("pad_zero(): Input & output images have imcompatible size.");

    auto slice = python::slice(pad_size, -pad_size);
    out(python::slice(pad_size)) = 0.0;
    out(python::slice(-pad_size, hi_out)) = 0.0;
    out(slice, python::slice(pad_size)) = 0.0;
    out(slice, python::slice(-pad_size, wid_out)) = 0.0;
    out(slice, slice) = image;
  }
  
  void convolve(const Image& image, const Kernel& kernel, Image& out) {
    
  }

}
