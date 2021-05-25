#include <aye/aye.hpp>
using namespace python;

int main() {
  try {
    
    auto image = aye::loadtxt("images/lena.txt");
    aye::imshow(aye::rotate2d(image, 127, 127, 64, 64, 192, 192, np::pi / 4.0), "");
        
  } catch(const std::exception& e) {
    print(e);
  }
}

