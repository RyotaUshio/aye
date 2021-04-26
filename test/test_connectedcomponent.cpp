#include <eyeball/eyeball.hpp>
#include <utility>
using namespace python;

int main() {
  try {
    eye::BinImage input({0, 0, 0, 0, 0, 0, 0, 0, 0,
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

    using namespace eye::algorithm;

    eye::ConnectedComponent4 result4(input);
    print(input);
    print(result4.label);
    
    eye::ConnectedComponent8 result8(input);
    print(input);
    print(result8.label);

  } catch (const std::exception& e) {
    print(e);
  }
}
