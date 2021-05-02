#include <aye/aye.hpp>
using namespace python;

int main(int argc, const char** argv) {
  try {    
    auto image = aye::imread(argc, argv, 4);
    
    auto dct = aye::dct(image);
    auto idct = aye::idct(dct);
    aye::savefig(argv[2], dct);
    aye::savefig(argv[3], idct);
    
  } catch (const std::exception& e) {
    print(e);
  }
}
