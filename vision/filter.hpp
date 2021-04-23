#pragma once

#include <vision/core.hpp>

#include <cmath> // std::exp
#include <numeric> // std::inner_product


namespace vision {
    
  Kernel Prewitt_x({-1, 0, 1,
		    -1, 0, 1,
		    -1, 0, 1}, {3, 3});
  Kernel Prewitt_y({-1, -1, -1,
		     0,  0,  0,
		    1,  1,  1}, {3, 3});
  Kernel Sobel_x({-1, 0, 1,
		  -2, 0, 2,
		  -1, 0, 1}, {3, 3});
  Kernel Sobel_y({-1, -2, -1,
		   0,  0,  0,
		  1,  2,  1}, {3, 3});

  Kernel Gauss(numpy::size_type sigma) {
    auto size = sigma * 2 + 1; // size of the kernel
    Kernel out = numpy::empty({size, size});
    for (int i=0; i<size; i++)
      for (int j=0; j<size; j++) {
	auto l2_dist = (i - sigma) * (i - sigma) + (j - sigma) * (j - sigma);
	out(i, j) = std::exp(-l2_dist / (2.0 * sigma * sigma));
      }
    return out;
  }
  
  void pad_zero(const Image& image, numpy::size_type pad_size, Image& out) {
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

  Image pad_zero(const Image& image, numpy::size_type pad_size) {
    Image out = numpy::empty({image.shape(0) + pad_size * 2,
			      image.shape(1) + pad_size * 2});
    pad_zero(image, pad_size, out);
    return out;
  }
  
  Image convolve(const Image& image, const Kernel& kernel) {
    numpy::size_type pad_size = kernel.shape(0) / 2;
    auto input = pad_zero(image, pad_size);
    auto shape = input.shape();
    auto output = numpy::empty(shape);

    for(int i=pad_size; i<shape[0]-pad_size; i++)
      for(int j=pad_size; j<shape[1]-pad_size; j++) {
	auto local = input(python::slice(i - pad_size, i + pad_size + 1),
			   python::slice(j - pad_size, j + pad_size + 1));
	output(i, j) = std::inner_product(local.begin(), local.end(), kernel.begin(), 0);
      }
    return output(python::slice(pad_size, -pad_size),
		  python::slice(pad_size, -pad_size)); // zero paddingした部分をもとに戻す
  }
}
