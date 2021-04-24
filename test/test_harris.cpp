#include <eyeball/eyeball.hpp>
using namespace python;
namespace np = numpy;

int main() {
  try {
    auto image = eyeball::sample::checkerboard;
    auto harris = eyeball::Harris(image, eyeball::Gauss(1), 3);
    eyeball::savetxt("./images/harris_response.txt", harris.response);
  } catch(const std::exception& e) {
    print(e);
  }
}
