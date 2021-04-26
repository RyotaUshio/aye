#include <eyeball/eyeball.hpp>
using namespace python;
namespace np = numpy;
namespace eye = eyeball;

int main(int argc, const char** argv) {
  try {
    if (argc != 5)
      throw std::runtime_error("args not enough");
    
    np::float_ sigma, th_low, th_high;
    sigma = std::atof(argv[2]);
    th_low = std::atof(argv[3]);
    th_high = std::atof(argv[4]);

    std::string name(argv[1]);
    auto lena = eye::loadtxt(("images/"+name+".txt").c_str());
    auto canny = eye::Canny(lena, sigma, th_low, th_high);

    eye::savetxt(("images/canny_"+name+"_strong_edge.txt").c_str(),
		 canny.threshold_result.strong_edge);
    eye::savetxt(("images/canny_"+name+"_may_be_edge.txt").c_str(),
		 canny.threshold_result.may_be_edge);
    eye::savetxt(("images/canny_"+name+"_grad.txt").c_str(),
		 canny.grad->magnitude);
    eye::savetxt(("images/canny_"+name+"_local_max.txt").c_str(),
		 canny.local_max);

    eye::savetxt(("images/canny_"+name+"_result.txt").c_str(), canny.result());
    
  } catch (const std::exception& e) {
    print(e);
  }
}
