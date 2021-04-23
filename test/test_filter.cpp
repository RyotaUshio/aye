#include <vision/vision.hpp>
using namespace python;
namespace np = numpy;

int main() {
  try {
    auto image = vision::sample::lena;
    auto prewitt = vision::Prewitt(image);
    auto center = vision::centered_diff(image);
    auto gauss = vision::convolve(image, vision::Gauss(1.4));
    auto log = vision::convolve(image, vision::LoG(1.4));
    
    vision::savetxt("images/lena.txt", image);
    vision::savetxt("images/lena_prewitt_x.txt", prewitt.diff_x);
    vision::savetxt("images/lena_prewitt_y.txt", prewitt.diff_y);
    vision::savetxt("images/lena_prewitt_mag.txt", prewitt.magnitude);
    vision::savetxt("images/lena_center_x.txt", center.diff_x);
    vision::savetxt("images/lena_center_y.txt", center.diff_y);
    vision::savetxt("images/lena_center_mag.txt", center.magnitude);
    vision::savetxt("images/lena_gauss.txt", gauss);
    vision::savetxt("images/lena_log.txt", log);
  } catch(const std::exception& e) {
    print(e);
  }
}
