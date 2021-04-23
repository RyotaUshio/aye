#include <vision/vision.hpp>
using namespace python;
namespace np = numpy;

int main() {
  try {
    auto image = vision::sample::corner;
    auto corner = vision::Harris(image, 3);
    vision::savetxt("./images/corner_response_gauss.txt", corner);
  } catch(const std::exception& e) {
    print(e);
  }
}
