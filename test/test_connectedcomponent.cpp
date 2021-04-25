#include <eyeball/eyeball.hpp>
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

    eye::ConnectedComponent4 result4(input);
    // print(input);
    // print(result4.label);
    // print(result4.lookup);
    
    eye::ConnectedComponent8 result8(input);
    // print(input);
    // print(result8.label);
    // print(result8.lookup);
    // print(result8.graph.vertices());

  } catch (const std::exception& e) {
    print(e);
  }
}
