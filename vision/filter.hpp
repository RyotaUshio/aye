#pragma once

#include <vision/core.hpp>

#include <cmath> // std::exp
#include <numeric> // std::inner_product


namespace vision {

 
  Kernel forward_diff_x({0, 0, 0,
			 0,-1, 1,
			 0, 0, 0}, {3, 3});
  Kernel forward_diff_y({0, 0, 0,
			 0, 1, 0,
			 0,-1, 0}, {3, 3});
  Kernel backward_diff_x({0, 0, 0,
			 -1, 1, 0,
			  0, 0, 0}, {3, 3});
  Kernel backward_diff_y({0, 1, 0,
			  0,-1, 0,
			  0, 0, 0}, {3, 3});
  Kernel centered_diff_x({0, 0, 0,
			 -1, 0, 1,
			  0, 0, 0}, {3, 3});
  Kernel centered_diff_y({0, 1, 0,
			  0, 0, 0,
			  0,-1, 0}, {3, 3});
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

  Kernel _l2_dist_sq(numpy::float64 sigma, numpy::float64 truncate) {
    numpy::size_type radius = std::round(truncate * sigma);
    numpy::size_type size = 2 * radius + 1; // size of the kernel
    Kernel out = numpy::empty({size, size});
    for (int i=0; i<size; i++)
      for (int j=0; j<size; j++)
	out(i, j) = (i - radius) * (i - radius) + (j - radius) * (j - radius);
    return out;
  }

  template <bool is_gauss>
  Kernel _Gauss_or_LoG(numpy::float64 sigma, numpy::float64 truncate) {
    auto tmp = 2.0 * sigma * sigma;
    auto x2 = _l2_dist_sq(sigma, truncate);
    auto out = numpy::exp(-x2 / tmp); // Gaussian
    if constexpr (is_gauss) {
      return out;
    } else {
      return out *= x2 - tmp; // LoG
    }
  }

  inline Kernel Gauss(numpy::float64 sigma, numpy::float64 truncate=4.0) {
    return _Gauss_or_LoG<true>(sigma, truncate);
  }
  
  inline Kernel LoG(numpy::float64 sigma, numpy::float64 truncate=4.0) {
    return _Gauss_or_LoG<false>(sigma, truncate);
  }

  Kernel Gauss_deriv_x(numpy::float64 sigma, numpy::float64 truncate=4.0) {
    return Gauss(sigma, truncate) *= 
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
    auto shape = image.shape();
    auto input = pad_zero(image, pad_size);
    auto output = numpy::empty(shape);

    for(int i=0; i<shape[0]; i++)
      for(int j=0; j<shape[1]; j++) {
	auto local = input(python::slice(i, i + 2 * pad_size + 1),
			   python::slice(j, j + 2 * pad_size + 1));
	output(i, j) = std::inner_product(local.begin(), local.end(), kernel.begin(), 0);
      }
    return output;
  }

  template <const Kernel& filter_x, const Kernel& filter_y>
  struct gradient {
    const Image& src;
    int magnitude_order;
    Image diff_x;
    Image diff_y;
    Image magnitude;
    Image direction;

    gradient(const Image& input, int order=2)
      : src(input), magnitude_order(order) {
      operator()();
    }

  private:
    void differentiate() {
      diff_x = convolve(src, filter_x);
      diff_y = convolve(src, filter_y);
    }

    void set_magnitude() {
      if (magnitude_order == 1)
	magnitude = numpy::fabs(diff_x) + numpy::fabs(diff_y);
      else if (magnitude_order == 2)
	magnitude = numpy::sqrt(diff_x * diff_x + diff_y * diff_y);
    }

    void set_direction() {
      direction = numpy::arctan2(diff_y, diff_x);
    }

    void operator()() {
      differentiate();
      set_magnitude();
      set_direction();
    }
  };

  using forward_diff = gradient<forward_diff_x, forward_diff_y>;
  using backward_diff = gradient<backward_diff_x, backward_diff_y>;
  using centered_diff = gradient<centered_diff_x, centered_diff_y>;
  using Prewitt = gradient<Prewitt_x, Prewitt_y>;
  using Sobel = gradient<Sobel_x, Sobel_y>;

}
