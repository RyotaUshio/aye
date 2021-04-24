#include <eyeball/eyeball.hpp>
using namespace python;
namespace np = numpy;
namespace eye = eyeball;

int main() {
  try {
    auto lena = eye::sample::checkerboard;
    auto canny = eye::Canny(lena, 1, 0, 0);
    eye::savetxt("images/canny.txt", canny.edge);
  } catch (const std::exception& e) {
    print(e);
  }
}
