#pragma once

namespace eyeball {

  struct Harris {
    const Image& input;
    const Kernel& window;
    numpy::float64 sigma; // はじめの平滑化のガウスフィルタ
    numpy::float64 k;

    /* results */
    Image response; // distribution of the corner response function
    // Image corner; // local maximum of response

    Harris(const Image& input_, const Kernel& window_, numpy::float64 sigma_, numpy::float64 k_=0.05)
      : input(input_), window(window_), sigma(sigma_), k(k_) {
      set_response();
      // set_corner();
    }
    
  private:
    void set_response() {
      auto I_x = convolve(input, Sobel_x);
      auto I_y = convolve(input, Sobel_y);
      auto I_xx = I_x * I_x;
      auto I_xy = I_x * I_y;
      auto I_yy = I_y * I_y;
      auto S_xx = convolve(I_xx, window);
      auto S_xy = convolve(I_xy, window);
      auto S_yy = convolve(I_yy, window);

      auto det = S_xx * S_yy - S_xy * S_xy;
      auto trace = S_xx + S_yy;
      response = det - k * trace * trace;
    }

    // non-maximum supression
    // void set_corner() {
    
    // }
    
  };

  // Image Canny(const Image& image, numpy::float64 sigma=1.4) {
    
  // }
  
}
