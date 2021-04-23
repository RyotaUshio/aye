#include <vision/vision.hpp>
using namespace python;
namespace np = numpy;

int main() {
  try {
    auto image = vision::sample::corner;
    auto harris = vision::Harris(image, vision::Gauss(1), 3);
    vision::savetxt("./images/harris_response.txt", harris.response);
  } catch(const std::exception& e) {
    print(e);
  }
}
