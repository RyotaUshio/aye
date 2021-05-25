#include <aye/aye.hpp>
using namespace python;

int main() {
  try {
    
    auto image = aye::loadtxt("images/nile1.txt");
    aye::savefig("images/nile1.bmp", image);
    aye::DoG DoG(image);
    // DoG.show_scale_space();
    // DoG.show_result();
    DoG.save_extrema("sift/nile1_DoG.txt");
        
  } catch(const std::exception& e) {
    print(e);
  }
}
