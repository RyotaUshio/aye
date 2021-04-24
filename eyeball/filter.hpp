#pragma once

#include <eyeball/core.hpp>

#include <cmath> // std::exp
#include <numeric> // std::inner_product


namespace eyeball {

 
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

  template <unsigned int which>
  Kernel _Gaussian_filters(numpy::float64 sigma, numpy::float64 truncate) {
    /**
     * which == 0 -> Gaussian
     * which == 1 -> x-derivative of Gaussian
     * which == 2 -> y-derivative of Gaussian
     * which == 3 -> LoG
     **/
    numpy::size_type radius = std::round(truncate * sigma);
    numpy::size_type size = 2 * radius + 1; // size of the kernel
    auto sigma_sq = sigma * sigma;
    auto tmp = 2.0 * sigma_sq;
    
    auto xy = numpy::indices<numpy::float64>({size, size}) - radius;
    auto x = xy(0);
    auto y = xy(1);
    auto x2_y2 = x*x + y*y;
    
    auto out = numpy::exp(-x2_y2 / tmp) / std::sqrt(numpy::pi * tmp); // Gaussian
    
    if constexpr (which == 0) {
      return out;
    } else if constexpr (which == 1) {
	return out *= x *= -1;// /= -sigma_sq;
    } else if constexpr (which == 2) {
	return out *= y *= -1;// /= -sigma_sq;
    } else if constexpr (which == 3) {
	return out *= x2_y2 - tmp;// /= (sigma_sq * sigma_sq); // LoG
    }
    static_assert(which < 4);
  }

  inline Kernel Gauss(numpy::float64 sigma, numpy::float64 truncate=4.0) {
    return _Gaussian_filters<0>(sigma, truncate);
  }

  inline Kernel Gauss_deriv_x(numpy::float64 sigma, numpy::float64 truncate=4.0) {
    return _Gaussian_filters<1>(sigma, truncate);
  }

  inline Kernel Gauss_deriv_y(numpy::float64 sigma, numpy::float64 truncate=4.0) {
    return _Gaussian_filters<2>(sigma, truncate);
  }

  inline Kernel LoG(numpy::float64 sigma, numpy::float64 truncate=4.0) {
    return _Gaussian_filters<3>(sigma, truncate);
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
    Image local;

    int i = 0;
    int j = 0;
    std::generate(output.begin(), output.end(),
    		  [&](){
    		    local = input(python::slice(i, i + 2 * pad_size + 1),
				  python::slice(j, j + 2 * pad_size + 1));
    		    j++; if (j == shape[1]) {j = 0; i++;}
    		    return std::inner_product(local.begin(), local.end(), kernel.begin(), 0);
    		  });
    return output;
  }

  template <class Func, class... Args>
  Image apply_local(const Image& image, int pad_size, Func func, Args... args) {
    auto shape = image.shape();
    Image input = pad_zero(image, pad_size);
    Image output = numpy::empty(shape);
    Image local;
    
    int i = 0; int j = -1;
    std::generate(output.begin(), output.end(),
    		  [&](){
		    j++; if (j == shape[1]) {j = 0; i++;}
    		    local = input(python::slice(i, i + 2 * pad_size + 1),
				  python::slice(j, j + 2 * pad_size + 1));
    		    return func(i, j, local, args...);
    		  });
    return output;
  }
  
  template <class Func, class... Args>
  Image apply(const numpy::shape_type& shape, Func func, Args... args) {
    auto output = numpy::empty(shape);
    int i = 0; int j = -1;
    std::generate(output.begin(), output.end(),
    		  [&](){
    		    j++; if (j == shape[1]) {j = 0; i++;}
    		    return func(i, j, args...);
    		  });
    return output;
  }

  Image unsharp_mask(const Image& input, numpy::float64 k=9.0,
		     numpy::float64 sigma=1.4, numpy::float64 truncate=4.0) {
    auto low_pass = convolve(input, Gauss(sigma, truncate));
    auto high_pass = input - low_pass;
    auto out = input + k * high_pass;
    return out;
  }

  struct gradient {
    const Image& src;
    const Kernel& filter_x;
    const Kernel& filter_y;	
    int magnitude_order;
    Image diff_x;
    Image diff_y;
    Image magnitude;
    Image direction;

    gradient(const Image& input, const Kernel& filter_x_, const Kernel& filter_y_, int order)
      : src(input), filter_x(filter_x_), filter_y(filter_y_), magnitude_order(order) {
      operator()();
    }

    virtual ~gradient() {}

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


  struct forward_diff : public gradient {
    forward_diff(const Image& input, int order=2) : gradient(input, forward_diff_x, forward_diff_y, order) {}
  };

  struct backward_diff : public gradient {
    backward_diff(const Image& input, int order=2) : gradient(input, backward_diff_x, backward_diff_y, order) {}
  };

  struct centered_diff : public gradient {
    centered_diff(const Image& input, int order=2) : gradient(input, centered_diff_x, centered_diff_y, order) {}
  };
  
  struct Prewitt : public gradient {
    Prewitt(const Image& input, int order=2) : gradient(input, Prewitt_x, Prewitt_y, order) {}
  };

  struct Sobel : public gradient {
    Sobel(const Image& input, int order=2) : gradient(input, Sobel_x, Sobel_y, order) {}
  };

  struct Gauss_gradient : public gradient {
    Gauss_gradient(const Image& input, numpy::float64 sigma=1.4, numpy::float64 truncate=4.0, int order=2)
      : gradient(input, Gauss_deriv_x(sigma, truncate), Gauss_deriv_y(sigma, truncate), order) {}
  };

  // template <const Kernel& filter_x, const Kernel& filter_y>
  // struct gradient {
  //   const Image& src;
  //   int magnitude_order;
  //   Image diff_x;
  //   Image diff_y;
  //   Image magnitude;
  //   Image direction;

  //   gradient(const Image& input, int order=2)
  //     : src(input), magnitude_order(order) {
  //     operator()();
  //   }

  // private:
  //   void differentiate() {
  //     diff_x = convolve(src, filter_x);
  //     diff_y = convolve(src, filter_y);
  //   }

  //   void set_magnitude() {
  //     if (magnitude_order == 1)
  // 	magnitude = numpy::fabs(diff_x) + numpy::fabs(diff_y);
  //     else if (magnitude_order == 2)
  // 	magnitude = numpy::sqrt(diff_x * diff_x + diff_y * diff_y);
  //   }

  //   void set_direction() {
  //     direction = numpy::arctan2(diff_y, diff_x);
  //   }

  //   void operator()() {
  //     differentiate();
  //     set_magnitude();
  //     set_direction();
  //   }
  // };

  
  // using forward_diff = gradient<forward_diff_x, forward_diff_y>;
  // using backward_diff = gradient<backward_diff_x, backward_diff_y>;
  // using centered_diff = gradient<centered_diff_x, centered_diff_y>;
  // using Prewitt = gradient<Prewitt_x, Prewitt_y>;
  // using Sobel = gradient<Sobel_x, Sobel_y>;

}
