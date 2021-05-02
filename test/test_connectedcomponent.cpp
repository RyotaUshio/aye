#include <aye/aye.hpp>
#include <utility>
using namespace python;

int main() {
  try {
    aye::BinImage input({0, 0, 0, 0, 0, 0, 0, 0, 0,
			 0, 0, 0, 0, 0, 0, 1, 0, 0,
			 0, 0, 1, 0, 0, 1, 1, 0, 0,
			 0, 1, 1, 0, 0, 0, 1, 0, 0,
			 0, 0, 1, 1, 0, 0, 1, 1, 0,
			 0, 0, 1, 1, 0, 1, 1, 0, 0,
			 0, 0, 1, 1, 1, 0, 0, 0, 0,
			 0, 0, 0, 1, 0, 0, 0, 0, 0,
			 0, 0, 0, 1, 1, 1, 0, 0, 0,
			 0, 0, 1, 1, 1, 1, 0, 0, 0,
			 0, 0, 0, 0, 0, 0, 0, 0, 0}, {11, 9});

    using namespace aye::algorithm;

    aye::ConnectedComponent4 result4(input);
    print(input);
    print(result4.label);
    
    aye::ConnectedComponent8 result8(input);
    print(input);
    print(result8.label);

  } catch (const std::exception& e) {
    print(e);
  }
}
