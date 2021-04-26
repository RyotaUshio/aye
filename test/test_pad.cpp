#include <eyeball/eyeball.hpp>
using namespace python;

int main() {
  try {
    eye::Image im = np::arange(1.0, 26.0).reshape(5, 5);
    print(im);
    print(eye::pad<eye::nearest>(im, 2));
    print(eye::pad<eye::constant>(im, 2, 100));
    print(eye::pad<eye::nearest>(im, 2));
  } catch (const std::exception& e) {
    print(e);
  }
}
