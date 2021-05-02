#include <aye/aye.hpp>
using namespace python;
namespace np = numpy;

int main() {
  try {
    auto image = aye::sample::checkerboard;
    auto harris = aye::Harris(image, aye::Gauss(1), 3);
    aye::savetxt("./images/harris_response.txt", harris.response);
  } catch(const std::exception& e) {
    print(e);
  }
}
