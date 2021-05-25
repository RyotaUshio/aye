#include <aye/aye.hpp>
using namespace python;
namespace np = numpy;

int main(int argc, const char* argv[]) {
  try {
    
    if (argc != 2)
      throw std::runtime_error("Invalid number of args");

    std::string name = argv[1];
    // auto image = aye::imread(("images/" + name + ".bmp").c_str());
    auto image = aye::loadtxt(("images/" + name + ".txt").c_str());

    auto harris = aye::Harris(image, aye::Gauss(1), 3);
    aye::savetxt(("./images/harris_response_" + name + ".txt").c_str(), harris.response);
    aye::imshow(harris.response, "");
    
  } catch(const std::exception& e) {
    print(e);
  }
}
