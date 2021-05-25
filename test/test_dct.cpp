#include <aye/aye.hpp>
using namespace python;

int main(int argc, const char** argv) {
  try {    
    if (argc != 2)
      throw std::runtime_error("Invalid number of args");

    std::string name = argv[1];
    auto image = aye::imread(("images/" + name + ".bmp").c_str());

    auto dct = aye::dct(image);
    auto idct = aye::idct(dct);
    aye::savefig(("images/dct_" + name + ".bmp").c_str(), dct);
    aye::savefig(("images/idct_" + name + ".bmp").c_str(), idct);
    aye::imshow(dct, "Result of DCT",
		idct, "Result of IDCT");
    
  } catch (const std::exception& e) {
    print(e);
  }
}
