#include <aye/aye.hpp>
using namespace python;

int main() {
  try {
    
    auto image = aye::loadtxt("images/lena.txt");
    aye::Pyramid p(image, 4);
    for (const auto& e : p.images)
      aye::imshow(e, "");
    
  } catch(const std::exception& e) {
    print(e);
  }
}


