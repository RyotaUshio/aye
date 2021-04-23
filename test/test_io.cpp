#include <eyeball/eyeball.hpp>
using namespace python;
namespace np = numpy;

int main() {
  try {
    eyeball::Image image = np::loadtxt("images/miri.txt");
    auto out = eyeball::Harris(image, 2);
    auto max = np::max(out);
    auto min = np::min(out);
    out = np::maximum(out, np::ndarray(max * 0.3 + min * 0.7));
    eyeball::savetxt("images/miri_harris.txt", out);
  } catch (const std::exception& e) {
    print(e);
  }
}
