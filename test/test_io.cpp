#include <aye/aye.hpp>
using namespace python;
namespace np = numpy;

int main() {
  try {
    auto image = np::imread("images/lena.bmp");
    aye::savetxt("images/miri_harris.txt", out);
  } catch (const std::exception& e) {
    print(e);
  }
}
