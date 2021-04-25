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

  struct Canny {
    const Image& input;
    numpy::float64 sigma;
    numpy::float64 th1;
    numpy::float64 th2;
    Gradient* grad;
    Image maximum;
    Image edge;
    
    Canny(const Image& input_,
	  numpy::float64 sigma_=1.4,
	  numpy::float64 th_low=30,
	  numpy::float64 th_high=70,
	  int order=2) : input(input_), sigma(sigma_) {
      // denoising & computing the gradient
      grad = new Sobel(convolve(input, Gauss(sigma)), order);
      // local maximum of the magnitude of gradient
      non_maximum_suppression();
      threshold();
    }

    void non_maximum_suppression() {
      auto tan = grad->diff_y / grad->diff_x;
      numpy::float64 t;
      numpy::float64 next1, next2;
      maximum = apply_local(grad->magnitude, 1,
			    [&](int i, int j, const Image& local){
			      t = tan[{i, j}];
			      if (t >= 1) {
				next1 = local[1] * (1.0 - 1.0/t) + local[2] / t;
				next2 = local[7] * (1.0 - 1.0/t) + local[6] / t;
			      } else if (t <= -1) {
				next1 = local[1] * (1.0 + 1.0/t) - local[0] / t;
				next2 = local[7] * (1.0 + 1.0/t) - local[8] / t;
			      } else if (0 <= t and t < 1) {
				next1 = local[5] * (1.0 - t) + local[2] * t;
				next2 = local[3] * (1.0 - t) + local[6] * t;
			      } else if (-1 < t and t < 0) {
				next1 = local[5] * (1.0 + t) - local[8] * t;
				next2 = local[3] * (1.0 + t) - local[0] * t;
			      }
			      return (next1 < local[4] and next2 < local[4]) ? 255.0 : 0.0;
			    });
    }

    void threshold() {
      edge = apply(maximum.shape(),
		   [this](int i, int j) {
		     auto g = grad->magnitude[{i, j}];
		     if (g < th1)
		       return 0.0;
		     else if (g > th2)
		       return 255.0;
		     else
		       return 0.0;
		   });
    }
    
    ~Canny() {
      delete grad;
    }
      
  };
  
}
