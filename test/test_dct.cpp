#include <eyeball/eyeball.hpp>
using namespace python;

int main(int argc, const char** argv) {
  try {    
    auto image = eye::imread(argc, argv, 4);
    
    auto dct = eye::dct(image);
    auto idct = eye::idct(dct);
    eye::savetxt(argv[2], dct);
    eye::savetxt(argv[3], idct);
    
  } catch (const std::exception& e) {
    print(e);
  }
}
