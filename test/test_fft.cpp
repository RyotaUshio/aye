#include <aye/aye.hpp>
using namespace python;

int main(int argc, const char** argv) {
  try {
    // the same example as
    // https://docs.scipy.org/doc/scipy/reference/generated/scipy.fft.fft2.html
    auto y = np::indices({5, 5})(0);
    print(y);
    print(aye::fft(y));
    print(np::real(aye::ifft(aye::fft(y))));
    
    auto image = aye::imread(argc, argv, 4);
    
    // FFT of an image
    auto fft = aye::Fourier(image);
    auto ifft = fft.inv();
    auto amp = fft.amplitude();
    aye::savetxt(argv[2], amp);
    aye::savetxt(argv[3], ifft);
    
  } catch (const std::exception& e) {
    print(e);
  }
}
