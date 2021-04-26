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
  Kernel _Gaussian_filters(np::float64 sigma, np::float64 truncate) {
    /**
     * which == 0 -> Gaussian
     * which == 1 -> x-derivative of Gaussian
     * which == 2 -> y-derivative of Gaussian
     * which == 3 -> LoG
     **/
    np::size_type radius = std::round(truncate * sigma);
    np::size_type size = 2 * radius + 1; // size of the kernel
    auto sigma_sq = sigma * sigma;
    auto tmp = 2.0 * sigma_sq;
    
    auto xy = np::indices<np::float64>({size, size}) - radius;
    auto x = xy(0);
    auto y = xy(1);
    auto x2_y2 = x*x + y*y;
    
    auto out = np::exp(-x2_y2 / tmp) / std::sqrt(np::pi * tmp); // Gaussian
    
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

  inline Kernel Gauss(np::float64 sigma, np::float64 truncate=4.0) {
    return _Gaussian_filters<0>(sigma, truncate);
  }

  inline Kernel Gauss_deriv_x(np::float64 sigma, np::float64 truncate=4.0) {
    return _Gaussian_filters<1>(sigma, truncate);
  }

  inline Kernel Gauss_deriv_y(np::float64 sigma, np::float64 truncate=4.0) {
    return _Gaussian_filters<2>(sigma, truncate);
  }

  inline Kernel LoG(np::float64 sigma, np::float64 truncate=4.0) {
    return _Gaussian_filters<3>(sigma, truncate);
  }

  enum PadMode {constant, nearest};
  
  template <PadMode mode, class Array>
  void pad_inplace_1d(Array&& input, np::size_type pad_size, typename Array::dtype fill_value) {
    using python::slice;
    slice head(pad_size);
    slice tail(-pad_size, input.size());
    
    if constexpr (mode == constant) {
	input(head) = fill_value;
	input(tail) = fill_value;
      } else { // mode == nearest
	input(head) = input(pad_size);
	input(tail) = input(-pad_size-1);
      }
  }

  template <PadMode mode, class Array>
  Array pad(const Array& input, np::size_type pad_size, typename Array::dtype fill_value=0) {
    Array out = np::empty<typename Array::dtype>({input.shape(0) + pad_size * 2,
						  input.shape(1) + pad_size * 2});
    python::slice center(pad_size, -pad_size);
    out(center, center) = input;
    
    for(int row=pad_size; row<input.shape(0)+pad_size; row++) 
      pad_inplace_1d<mode>(out(row), pad_size, fill_value);

    for(np::axis_type ax=1; ax<input.ndim(); ax++) {
      auto tmp = np::utils::bring_axis_to_head(out, ax);
      for(int i=0; i<tmp.shape(0); i++) 
	pad_inplace_1d<mode>(tmp(i), pad_size, fill_value);
    }
    
    return out;
  }

  template <class Array, class PadFunc>
  Array pad_apply(const Array& image, np::size_type pad_size, PadFunc func) {
    Array out = np::empty<typename Array::dtype>({image.shape(0) + pad_size * 2,
						  image.shape(1) + pad_size * 2});
    auto hi = image.shape(0); // height of the input image
    auto wid = image.shape(1); // width of the input image
    auto hi_out = out.shape(0); // height of the output image
    auto wid_out = out.shape(1); // width of the output image

    if ((hi + pad_size *2 != hi_out) or (wid + pad_size * 2 != wid_out))
      throw std::invalid_argument("pad_zero(): Input & output images have imcompatible size.");

    using python::slice;
    slice center(pad_size, -pad_size);
    slice head(pad_size);
    slice tail_h(-pad_size, hi_out);
    slice tail_w(-pad_size, wid_out);
    out(center, center) = image;
    
    out(head) = func(image(head));     // top
    out(tail_h) = func(image(tail_h)); // bottom
    out(":", head) = func(image(":", head));     // left
    out(":", tail_w) = func(image(":", tail_w)); // right

    return out;
  }
  
  template <class Array>
  Array pad_zero(const Array& image, np::size_type pad_size) {
    return pad_apply(image, pad_size, [](auto x) -> typename decltype(x)::dtype {return 0;});
  }

  
  template <class Array>
  Array pad_nearest(const Array& image, np::size_type pad_size) {
    return pad_apply(image, pad_size,
		     [pad_size](auto x) -> decltype(x) {
		       decltype(x) out;
		       auto H = x.shape(0);
		       auto W = x.shape(1);
		       if (H < W) {
		       	 out = np::empty<typename decltype(x)::dtype>({H, W + pad_size * 2});
			 out(":", python::slice(pad_size, -pad_size)) = x;
		       }
		       else {
		       	 out = np::empty<typename decltype(x)::dtype>({H + pad_size * 2, W});
			 out(python::slice(pad_size, -pad_size)) = x;
		       }
		       return out;
		     });
  }



  
  Image convolve(const Image& image, const Kernel& kernel) {
    np::size_type pad_size = kernel.shape(0) / 2;
    auto shape = image.shape();
    auto input = pad_zero(image, pad_size);
    auto output = np::empty(shape);
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
    Image output = np::empty(shape);
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
  
  template <class Dtype=np::float_, class Func, class... Args>
  np::ndarray<Dtype> apply(const np::shape_type& shape, Func func, Args... args) {
    auto output = np::empty<Dtype>(shape);
    int i = 0; int j = -1;
    std::generate(output.begin(), output.end(),
    		  [&](){
    		    j++; if (j == shape[1]) {j = 0; i++;}
    		    return func(i, j, args...);
    		  });
    return output;
  }

  Image unsharp_mask(const Image& input, np::float64 k=9.0,
		     np::float64 sigma=1.4, np::float64 truncate=4.0) {
    auto low_pass = convolve(input, Gauss(sigma, truncate));
    auto high_pass = input - low_pass;
    auto out = input + k * high_pass;
    return out;
  }

  Image bilateral(const Image& input,
		  np::float_ sigma1, np::float_ sigma2,
		  np::float64 truncate=4.0) {
    auto kernel = Gauss(sigma1, truncate);
    auto radius = kernel.shape(0) / 2;
    Image::dtype center;
    return apply_local(input, radius, 
		       [&](int i, int j, const Image& local) {
			 center = input[{i, j}];
			 kernel *= (np::exp(np::square(local - center) / (-2.0 * sigma2 * sigma2)));
			 return std::inner_product(local.begin(), local.end(), kernel.begin(), 0);
		       });
  }

  struct Gradient {
    const Image& src;
    const Kernel& filter_x;
    const Kernel& filter_y;	
    int magnitude_order;
    Image diff_x;
    Image diff_y;
    Image magnitude;
    Image direction;

    Gradient(const Image& input, const Kernel& filter_x_, const Kernel& filter_y_, int order)
      : src(input), filter_x(filter_x_), filter_y(filter_y_), magnitude_order(order) {
      operator()();
    }

    virtual ~Gradient() {}

  private:
    void differentiate() {
      diff_x = convolve(src, filter_x);
      diff_y = convolve(src, filter_y);
    }

    void set_magnitude() {
      if (magnitude_order == 1)
  	magnitude = np::fabs(diff_x) + np::fabs(diff_y);
      else if (magnitude_order == 2)
  	magnitude = np::sqrt(diff_x * diff_x + diff_y * diff_y);
    }

    void set_direction() {
      direction = np::arctan2(diff_y, diff_x);
    }

    void operator()() {
      differentiate();
      set_magnitude();
      set_direction();
    }
  };


  struct ForwardDiff : public Gradient {
    ForwardDiff(const Image& input, int order=2) : Gradient(input, forward_diff_x, forward_diff_y, order) {}
  };

  struct BackwardDiff : public Gradient {
    BackwardDiff(const Image& input, int order=2) : Gradient(input, backward_diff_x, backward_diff_y, order) {}
  };

  struct CenteredDiff : public Gradient {
    CenteredDiff(const Image& input, int order=2) : Gradient(input, centered_diff_x, centered_diff_y, order) {}
  };
  
  struct Prewitt : public Gradient {
    Prewitt(const Image& input, int order=2) : Gradient(input, Prewitt_x, Prewitt_y, order) {}
  };

  struct Sobel : public Gradient {
    Sobel(const Image& input, int order=2) : Gradient(input, Sobel_x, Sobel_y, order) {}
  };

  struct GaussGradient : public Gradient {
    GaussGradient(const Image& input, np::float64 sigma=1.4, np::float64 truncate=4.0, int order=2)
      : Gradient(input, Gauss_deriv_x(sigma, truncate), Gauss_deriv_y(sigma, truncate), order) {}
  };

}
