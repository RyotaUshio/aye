#include <eyeball/eyeball.hpp>
using namespace python;

int main() {
  try {
    auto image = eye::loadtxt("images/lena.txt");
    auto prewitt = eye::Prewitt(image);
    auto center = eye::CenteredDiff(image);
    auto gauss = eye::convolve(image, eye::Gauss(1.4));
    auto gauss_grad = eye::GaussGradient(image);
    auto log = eye::convolve(image, eye::LoG(1.4));
    auto sharp = eye::unsharp_mask(image, 4.0, 2.2);
    // auto bilateral1 = eye::bilateral(image, 8, 0.2);
    // auto bilateral2 = eye::bilateral(image, 8, 1.0);
    
    eye::savetxt("images/lena_prewitt_x.txt", prewitt.diff_x);
    eye::savetxt("images/lena_prewitt_y.txt", prewitt.diff_y);
    eye::savetxt("images/lena_prewitt_mag.txt", prewitt.magnitude);
    eye::savetxt("images/lena_center_x.txt", center.diff_x);
    eye::savetxt("images/lena_center_y.txt", center.diff_y);
    eye::savetxt("images/lena_center_mag.txt", center.magnitude);
    eye::savetxt("images/lena_gauss.txt", gauss);
    eye::savetxt("images/lena_gauss_grad_x.txt", gauss_grad.diff_x);
    eye::savetxt("images/lena_gauss_grad_y.txt", gauss_grad.diff_y);
    eye::savetxt("images/lena_gauss_grad_mag.txt", gauss_grad.magnitude);
    eye::savetxt("images/lena_log.txt", log);
    eye::savetxt("images/lena_sharp.txt", sharp);
    // eye::savetxt("images/lena_bilateral_8-0_5.txt", bilateral1);
    // eye::savetxt("images/lena_bilateral_8-1_0.txt", bilateral2);
  } catch(const std::exception& e) {
    print(e);
  }
}
