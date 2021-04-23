#pragma once

namespace vision {

  Image Harris(const Image& image, numpy::size_type sigma, numpy::float64 k=0.06) {
    auto I_x = convolve(image, Sobel_x);
    auto I_y = convolve(image, Sobel_y);
    auto I_xx = I_x * I_x;
    auto I_xy = I_x * I_y;
    auto I_yy = I_y * I_y;
    auto gauss = Gauss(sigma);
    // auto gauss = numpy::ones({sigma * 2 + 1, sigma * 2 + 1});
    auto S_xx = convolve(I_xx, gauss);
    auto S_xy = convolve(I_xy, gauss);
    auto S_yy = convolve(I_yy, gauss);

    auto det = S_xx * S_yy - S_xy * S_xy;
    auto trace = S_xx + S_yy;
    auto trace_sq = trace * trace;
    trace_sq *= k;
    auto C = det - trace_sq; // corner response function
    
    return C;
  }
  
}
