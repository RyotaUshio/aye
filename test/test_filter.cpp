#include <eyeball/eyeball.hpp>
using namespace python;
namespace np = numpy;

int main() {
  try {
    auto image = eyeball::sample::lena;
    auto prewitt = eyeball::Prewitt(image);
    auto center = eyeball::centered_diff(image);
    auto gauss = eyeball::convolve(image, eyeball::Gauss(1.4));
    auto gauss_grad = eyeball::Gauss_gradient(image);
    auto log = eyeball::convolve(image, eyeball::LoG(1.4));
    auto sharp = eyeball::unsharp_mask(image, 4.0, 2.2);
    
    eyeball::savetxt("images/lena.txt", image);
    eyeball::savetxt("images/lena_prewitt_x.txt", prewitt.diff_x);
    eyeball::savetxt("images/lena_prewitt_y.txt", prewitt.diff_y);
    eyeball::savetxt("images/lena_prewitt_mag.txt", prewitt.magnitude);
    eyeball::savetxt("images/lena_center_x.txt", center.diff_x);
    eyeball::savetxt("images/lena_center_y.txt", center.diff_y);
    eyeball::savetxt("images/lena_center_mag.txt", center.magnitude);
    eyeball::savetxt("images/lena_gauss.txt", gauss);
    eyeball::savetxt("images/lena_gauss_grad_x.txt", gauss_grad.diff_x);
    eyeball::savetxt("images/lena_gauss_grad_y.txt", gauss_grad.diff_y);
    eyeball::savetxt("images/lena_gauss_grad_mag.txt", gauss_grad.magnitude);
    eyeball::savetxt("images/lena_log.txt", log);
    eyeball::savetxt("images/lena_sharp.txt", sharp);
  } catch(const std::exception& e) {
    print(e);
  }
}
