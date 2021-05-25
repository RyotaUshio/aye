#include <aye/aye.hpp>
using namespace python;
namespace np = numpy;


int main(int argc, const char** argv) {
  try {
    if (argc != 5)
      throw std::runtime_error("args not enough");
    
    np::float_ sigma, th_low, th_high;
    sigma = std::atof(argv[2]);
    th_low = std::atof(argv[3]);
    th_high = std::atof(argv[4]);

    std::string name(argv[1]);
    auto lena = aye::loadtxt(("images/"+name+".txt").c_str());
    auto canny = aye::Canny(lena, sigma, th_low, th_high);

    aye::savefig(("images/canny_"+name+"_strong_edge.bmp").c_str(),
		 canny.threshold_result.strong_edge);
    aye::savefig(("images/canny_"+name+"_may_be_edge.bmp").c_str(),
		 canny.threshold_result.may_be_edge);
    aye::savefig(("images/canny_"+name+"_grad.bmp").c_str(),
		 canny.grad->magnitude);
    aye::savefig(("images/canny_"+name+"_local_max.bmp").c_str(),
		 canny.local_max);
    aye::savefig(("images/canny_"+name+"_result.bmp").c_str(), 1 - canny.result());
    aye::imshow(1 - canny.result(), "Result", canny.grad->magnitude, "Gradient");
    
  } catch (const std::exception& e) {
    print(e);
  }
}
