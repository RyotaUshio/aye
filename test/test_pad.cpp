#include <aye/aye.hpp>
using namespace python;

int main() {
  try {
    aye::Image im = np::arange(1.0, 26.0).reshape(5, 5);
    print(im);
    print(aye::pad<aye::nearest>(im, 2));
    print(aye::pad<aye::constant>(im, 2, 100));
    print(aye::pad<aye::nearest>(im, 2));
  } catch (const std::exception& e) {
    print(e);
  }
}
