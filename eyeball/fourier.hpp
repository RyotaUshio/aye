#pragma once

#include <eyeball/core.hpp>

namespace eyeball {

  np::ndarray<np::complex_> _fft_rotation_factor(np::size_type N) {
    /**
     *  N > 0 corresponds to FFT, and N < 0 IFFT.
     */
    double coef = 2.0 * np::pi / static_cast<double>(N);
    return apply<np::complex_>({std::abs(N/2)},
			       [coef](int _, int k) {
				 return np::complex_(std::cos(coef*k), -std::sin(coef*k));
			       });
  }

  np::ndarray<np::complex_> dft_matrix(np::size_type N) {
    /**
     *  N > 0 corresponds to FFT, and N < 0 IFFT.
     */
    double coef = 2.0 * np::pi / static_cast<double>(N);
    return apply<np::complex_>({std::abs(N), std::abs(N)},
			       [coef](int i, int j) {
				 return np::complex_(std::cos(coef*i*j), -std::sin(coef*i*j));
			       });
  }

  template <bool inverse>
  auto _fft1d_impl(const np::ndarray<np::complex_>& x,
		   const np::ndarray<np::complex_>& W)
    -> np::ndarray<np::complex_> {
    /**
     * x : input signal of size N
     * W : rotation factors (W_N^k (k = 0, 1, ..., N/2))
     **/
    
    auto N = x.size(); // length of the input signal

    if (N == 1) return x;
    
    if (x.ndim() != 1) 
      throw std::invalid_argument("fft1d must take an 1-d array as arg");

    auto out = np::empty<np::complex_>({N}); // the output
    
    if (N % 2 == 0) {
      auto g = x(python::slice(0, N, 2)); // even-numbered part of the input
      auto h = x(python::slice(1, N + 1, 2)); // odd-numbered part of the input
      auto w = W("::2"); // W = W_N^k (k = 0, 1, ..., N/2) -> w = W_{N/2}^k (k = 0, 1, ..., N/4)
      auto G = _fft1d_impl<inverse>(g, w); // FFT of even-numbered part
      auto H = _fft1d_impl<inverse>(h, w); // FFT of odd-numbered part
     
      out(python::slice(0, N / 2)) = G + W * H; // 1st half of the output
      out(python::slice(N / 2, N)) = G - W * H; // 2nd half of the output

    } else {
      // naive DFT
      if constexpr (inverse) {
	return np::matmul(dft_matrix(-N), x);
      }
      else {
	return np::matmul(dft_matrix(N), x);
      }
    }

    return out;
  }

  template <bool inverse, class Dtype>
  np::ndarray<np::complex_> _fft1d(const np::ndarray<Dtype>& x) {
    np::size_type N = x.size();
    auto x_ = x.template astype<np::complex_>();
    np::ndarray<np::complex_> W;
    if constexpr (inverse) {
	W = _fft_rotation_factor(-N);
      } else {
      W = _fft_rotation_factor(N);
    }
    auto out = _fft1d_impl<inverse>(x_, W);
    if constexpr (inverse) {
	return out / N;
      } else {
      return out;
    }	
  }

  template <bool inverse, class Dtype>
  np::ndarray<np::complex_> _fftnd(const np::ndarray<Dtype>& x) {
    auto out = x.template astype<np::complex_>();
    
    for(int ax=0; ax<x.ndim(); ax++) {
      auto tmp = np::utils::bring_axis_to_head(out, ax);
      for(int k=0; k<x.shape(0); k++)
	tmp(k) = _fft1d<inverse>(tmp(k).copy());
    }
    // .copy()を消すとおかしい動作をする。なぜ？
    return out;
  }

  template <class Dtype>
  np::ndarray<Dtype> _fftshift_impl(const np::ndarray<Dtype>& x) {
    auto out = np::empty<Dtype>(x.shape());
    int size = out.shape(0);

    python::slice first_half(size / 2), second_half(size / 2, size);
    out(first_half) = x(second_half);
    out(second_half) = x(first_half);
    return out;
  }

  template <class Dtype>
  np::ndarray<Dtype> fftshift(const np::ndarray<Dtype>& x) {
    auto out = x.copy();
    for (int ax=0; ax<x.ndim(); ax++) {
      auto tmp = np::utils::bring_axis_to_head(out, ax);
      out = _fftshift_impl(tmp);
    }
    return out;
  }

  template <class Array>
  inline auto fft(const Array& x) -> np::ndarray<np::complex_> {
    return _fftnd<false, typename Array::dtype>(x);
  }

  template <class Array>
  inline auto ifft(const Array& x) -> np::ndarray<np::complex_> {
    return _fftnd<true, typename Array::dtype>(x);
  }

  
  struct Fourier {
    const CImage result;

    Fourier(const Image& input) : result(fft(input)) {}
    Image amplitude() const {
      auto dB = 20.0 * np::log10( np::abs(result) );
      return fftshift(dB);
    }
    Image inv() const {
      return np::real(ifft(result));
    }
  };
  
}
