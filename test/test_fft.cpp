#include <eyeball/eyeball.hpp>
using namespace python;

int main(int argc, const char** argv) {
  try {
    auto y = np::indices({5, 5})(0);
    print(y);
    print(eye::fft(y));
    
    auto image = eye::imread(argc, argv, 4);
    auto fft = eye::fft(image);
    auto ifft = eye::ifft(fft);
    print(np::sum(np::abs(np::real(ifft))));
    print(np::sum(np::abs(np::imag(ifft))));
    eye::savetxt(argv[2], 20 * np::log10(np::abs(eye::fftshift(fft))));
    eye::savetxt(argv[3], np::real(ifft));
    
  } catch (const std::exception& e) {
    print(e);
  }
}
