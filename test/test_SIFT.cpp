#include <aye/aye.hpp>
using namespace python;

int main(int argc, const char** argv) {
  try {

    if (argc != 3) {
      print("usage: test_SIFT <input> <output>");
      std::exit(1);
    }
    
    auto image = aye::loadtxt(argv[1]);

    print("hello");
    
    aye::SIFT sift(image);

    print("hello");
    
    sift.save(argv[2]);
    
    print("hello");
    
  } catch(const std::exception& e) {
    print(e);
  }
}
