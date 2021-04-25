#include <eyeball/eyeball.hpp>
using namespace python;

int main() {
  try {
    using namespace eye::algorithm;
    Vertex v1(1, 1), v2(2, 2), v3(3, 3), v4(4, 4), v5(5, 5), v6(6, 6), v7(7, 7);
    
    auto graph1 = DirectedGraph<int>::from_edges({Edge(v1, v2), Edge(v1, v5),
						  Edge(v2, v3), Edge(v2, v6),
						  Edge(v3, v6), Edge(v4, v3),
						  Edge(v5, v6), Edge(v7, v4),
						  Edge(v7, v6)});
    auto graph2 = UndirectedGraph<int>::from_edges({Edge(v1, v2), Edge(v1, v5),
						    Edge(v2, v3), Edge(v2, v6),
						    Edge(v3, v6), Edge(v4, v3),
						    Edge(v5, v6), Edge(v7, v4),
						    Edge(v7, v6)});
    
    graph1.connected_components();
    print(graph1.vertices());
    graph2.connected_components();
    print(graph2.vertices());

  } catch (const std::exception& e) {
    print(e);
  }
}
