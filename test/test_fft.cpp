#include <eyeball/eyeball.hpp>
using namespace python;

int main(int argc, const char** argv) {
  try {
    auto y = np::indices({5, 5})(0);
    print(y);
    print(eye::fft(y));
    print(np::real(eye::ifft(eye::fft(y))));
    
    auto image = eye::imread(argc, argv, 4);
    auto fft = eye::Fourier(image);
    auto ifft = fft.inv();
    auto amp = fft.amplitude();
    eye::savetxt(argv[2], amp);
    eye::savetxt(argv[3], ifft);
    
  } catch (const std::exception& e) {
    print(e);
  }
}
