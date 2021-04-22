#include <vision/vision.hpp>
using namespace python;
namespace np = numpy;

int main() {
  try {
    auto image = vision::sample::corner;
    auto corner = vision::Harris(image, 1);
    vision::savetxt("./images/corner_response_5.txt", corner);
  } catch(const std::exception& e) {
    print(e);
  }
}
