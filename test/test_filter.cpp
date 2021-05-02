#include <aye/aye.hpp>
using namespace python;

int main() {
  try {
    auto image = aye::loadtxt("images/lena.txt");
    auto prewitt = aye::Prewitt(image);
    auto center = aye::CenteredDiff(image);
    auto gauss = aye::convolve(image, aye::Gauss(1.4));
    auto gauss_grad = aye::GaussGradient(image);
    auto log = aye::convolve(image, aye::LoG(1.4));
    auto sharp = aye::unsharp_mask(image, 4.0, 2.2);
    // auto bilateral1 = aye::bilateral(image, 8, 0.2);
    // auto bilateral2 = aye::bilateral(image, 8, 1.0);

    aye::savetxt("images/lena_prewitt_x.txt", prewitt.diff_x);
    aye::savetxt("images/lena_prewitt_y.txt", prewitt.diff_y);
    aye::savetxt("images/lena_prewitt_mag.txt", prewitt.magnitude);
    aye::savetxt("images/lena_center_x.txt", center.diff_x);
    aye::savetxt("images/lena_center_y.txt", center.diff_y);
    aye::savetxt("images/lena_center_mag.txt", center.magnitude);
    aye::savetxt("images/lena_gauss.txt", gauss);
    aye::savetxt("images/lena_gauss_grad_x.txt", gauss_grad.diff_x);
    aye::savetxt("images/lena_gauss_grad_y.txt", gauss_grad.diff_y);
    aye::savetxt("images/lena_gauss_grad_mag.txt", gauss_grad.magnitude);
    aye::savetxt("images/lena_log.txt", log);
    aye::savetxt("images/lena_sharp.txt", sharp);
    // aye::savetxt("images/lena_bilateral_8-0_5.txt", bilateral1);
    // aye::savetxt("images/lena_bilateral_8-1_0.txt", bilateral2);
  } catch(const std::exception& e) {
    print(e);
  }
}
