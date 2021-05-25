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

    if (argc != 2)
      throw std::runtime_error("Invalid number of args");

    std::string name = argv[1];
    auto image = aye::imread(("images/" + name + ".bmp").c_str());
    
    // FFT of an image
    auto fft = aye::Fourier(image);
    auto ifft = fft.inv();
    auto amp = fft.amplitude();
    aye::savefig(("images/fft_" + name + ".bmp").c_str(), amp);
    aye::savefig(("images/ifft_" + name + ".bmp").c_str(), ifft);
    aye::imshow(amp, "Amplitude spectrum",
		ifft, "Result of IFFT");
    
  } catch (const std::exception& e) {
    print(e);
  }
}
