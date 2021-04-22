#include <vision/vision.hpp>
using namespace python;
namespace np = numpy;

int main() {
  try {
    auto image = vision::sample::lena;
    auto diff_x = vision::convolve(image, vision::Prewitt_x);
    auto diff_y = vision::convolve(image, vision::Prewitt_y);
    auto grad = diff_x * diff_x + diff_y * diff_y;
    auto gauss5 = vision::convolve(image, vision::Gauss(5));
    auto gauss20 = vision::convolve(image, vision::Gauss(20));

    // vision::Image diff_x, diff_y, grad, gauss5, gauss20;
    // print(np::debug::timeit([&](){diff_x = vision::convolve(image, vision::Prewitt_x);}));
    // print(np::debug::timeit([&](){diff_y = vision::convolve(image, vision::Prewitt_y);}));
    // print(np::debug::timeit([&](){grad = diff_x * diff_x + diff_y * diff_y;}));
    // print(np::debug::timeit([&](){gauss5 = vision::convolve(image, vision::Gauss(5));}));
    // print(np::debug::timeit([&](){gauss20 = vision::convolve(image, vision::Gauss(20));}));
    
    vision::savetxt("lena.txt", image);
    vision::savetxt("lena_diff_x.txt", diff_x);
    vision::savetxt("lena_diff_y.txt", diff_y);
    vision::savetxt("lena_grad.txt", grad);
    vision::savetxt("lena_gauss5.txt", gauss5);
    vision::savetxt("lena_gauss20.txt", gauss20);
  } catch(const std::exception& e) {
    print(e);
  }
}
