#pragma once

namespace aye {

  using python::print;

  np::ndarray<np::float_> rotation_mat2d(np::float_ theta) {
    auto cos = std::cos(theta);
    auto sin = std::sin(theta);
    return np::ndarray<np::float_>({cos, -sin,
				    sin, cos}, {2, 2});
  }

  np::float_ interpolate_nearest(const Image& input, np::float_ x, np::float_ y) {
    int x_ = std::round(x);
    int y_ = std::round(y);
    if ( (0 <= x_ and x_ < input.shape(0))
	 and
	 (0 <= y_ and y_ < input.shape(1)) ) 
      return input[{x_, y_}];
    else
      return 0;
  }
  
  np::float_ interpolate_bilinear(const Image& input, np::float_ x, np::float_ y) {
    int x_, y_;
    x_ = int(x); // [x]
    y_ = int(y); // [y]
    np::float_ ret = 0;

    if ( (0 <= x and x_ + 1 < input.shape(0))
	 and
	 (0 <= y and y_ + 1 < input.shape(1)) ) {
      ret += (x_ + 1 - x) * (y_ + 1 - y) * input[{x_, y_}];
      ret += (x_ + 1 - x) * (y - y_) * input[{x_, y_ + 1}];
      ret += (x - x_) * (y_ + 1 - y) * input[{x_ + 1, y_}];
      ret += (x - x_) * (y - y_) * input[{x_ + 1, y_ + 1}];
    }
    return ret;
  }
 

  Image rotate2d(const Image& input,
		 np::float_ x_center, np::float_ y_center, 
		 int x_upper_left, int y_upper_left,
		 int x_lower_right, int y_lower_right,
		 np::float_ theta) {
    np::shape_type shape{x_lower_right - x_upper_left, y_lower_right - y_upper_left};
    auto indices = np::indices(shape);
    indices(0) += x_upper_left;
    indices(1) += y_upper_left;
    auto coords = indices.reshape({2, shape[0] * shape[1]});
    // a matrix of coordinate vectors, for example:
    // 0 0 1 1
    // 0 1 0 1
    auto center = np::ndarray<np::float_>({x_center, y_center}, {2, 1});
    
    auto inv_coords = np::matmul(rotation_mat2d(-theta), coords - center) + center;
    auto output = np::empty(shape);

    auto it = output.begin();
    auto end = output.end();
    int i = 0;

    for (; it!=end; it++, i++) {
      auto xy = inv_coords(":", i);
      auto x = *xy.begin();
      auto y = *(xy.begin() + 1);
      *it = interpolate_bilinear(input, x, y);
    }

    return output;
  }

}
