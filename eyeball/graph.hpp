#pragma once

#include <eyeball/eyeball.hpp>
#include <map>
#include <utility>
#include <algorithm>
#include <initializer_list>

namespace eyeball {

  namespace algorithm {

    using python::print;
    using python::str;

    template <class T, class Label_t=int>
    struct Vertex {
      
      using type = T;
      using label_type = Label_t;
      
      const T key;
      Label_t label;
      const Label_t original_label;

      Vertex(T key_, Label_t label_) : key(key_), label(label_), original_label(label) {}
      Vertex(T key_) : Vertex(key_, Label_t(0)) {}
      Vertex(const Vertex<T, Label_t>& rhs) : Vertex(rhs.key, rhs.label) {}
      
      bool operator<(const Vertex<T, Label_t>& rhs) const {
	return key < rhs.key;
      }
      
      bool operator==(const Vertex<T, Label_t>& rhs) const {
	return key == rhs.key;
	
      }
      std::string __repr__() const {
	using python::str;
	return "Vertex<" + str(typeid(T)) + ", " + str(typeid(Label_t))+ ">(" +
	  "key=" + str(key) + ", label=" + str(label) + ")";
      }
      
    };


    template <class T, class Label_t=int>
    struct Edge {

      using Vertex_t = Vertex<T, Label_t>;
      
      Vertex_t& first;
      Vertex_t& second;
      
      Label_t label;

      Edge(Vertex_t& first_, Vertex_t& second_, Label_t lab)
      	: first(first_), second(second_), label(lab) {}
      Edge(Vertex_t& first_, Vertex_t& second_)
      	: Edge(first_, second_, Label_t()) {}

      bool operator<(const Edge<T, Label_t>& rhs) const {
    	return first < rhs.first or second < rhs.second;
      }
      
      bool operator==(const Edge<T, Label_t>& rhs) const {
    	return first == rhs.first and second == rhs.second;
      }
      
      std::string __repr__() const {
    	using python::str;
    	return "Edge<" + str(typeid(T)) + ", " + str(typeid(Label_t))+ ">(" +
    	  "first=" + str(first) + ", second=" + str(second) + ")";	  
      }
    };

    // template <class T, class Label_t=int>
    // struct Edge {

    //   using Vertex_t = Vertex<T, Label_t>;
      
    //   Vertex_t first;
    //   Vertex_t second;
      
    //   Label_t label;

    //   Edge(const Vertex_t& first_, const Vertex_t& second_, Label_t lab)
    //   	: first(first_), second(second_), label(lab) {}
    //   Edge(const Vertex_t& first_, const Vertex_t& second_)
    //   	: Edge(first_, second_, Label_t()) {}

    //   bool operator<(const Edge<T, Label_t>& rhs) const {
    // 	return first < rhs.first or second < rhs.second;
    //   }
      
    //   bool operator==(const Edge<T, Label_t>& rhs) const {
    // 	return first == rhs.first and second == rhs.second;
    //   }
      
    //   std::string __repr__() const {
    // 	using python::str;
    // 	return "Edge<" + str(typeid(T)) + ", " + str(typeid(Label_t))+ ">(" +
    // 	  "first=" + str(first) + ", second=" + str(second) + ")";	  
    //   }
    // };


    template <bool directed, class T, class Label_t> struct Graph;
    template <class T, class Label_t=int> using DirectedGraph = Graph<true, T, Label_t>;
    template <class T, class Label_t=int> using UndirectedGraph = Graph<false, T, Label_t>;

    
    // template <bool directed, class T, class Label_t=int>
    // struct Graph {

    //   using Vertex_t = Vertex<T, Label_t>;
    //   using Vertex_set = std::vector<Vertex_t>;
    //   using Vertex_ptr = typename Vertex_set::iterator;
    //   using Vertex_ptr_set = std::set<Vertex_ptr>;
    //   using Edge_t = Edge<T, Label_t>;
    //   using Edge_set = std::set<Edge_t>;
    //   using AdjList = std::map<Vertex_ptr, Vertex_ptr_set>;

    //   Vertex_set vertices;
    //   AdjList adj_list;

    //   Graph() = default;
    //   Graph(const Edge_set& edges) { 
    // 	for (const auto& edge : edges) {

    // 	  print(vertices);
	  
    // 	  Vertex_ptr first_ptr = add_if_new(edge.first); // copy and insert the vertex to the vertices set
    // 	  Vertex_ptr second_ptr = add_if_new(edge.second);

    // 	  print(edge.first, ">", *first_ptr);
    // 	  print(edge.second, ">", *second_ptr);
	  
    // 	  adj_list[first_ptr].insert(second_ptr);	  
    // 	  if constexpr (not directed) {
    // 	    adj_list[second_ptr].insert(first_ptr);
    // 	    }	
    // 	}
    //   }
      
    //   Vertex_ptr add_if_new(const Vertex_t& v) {
    // 	auto it = std::find(vertices.begin(), vertices.end(), v);
    // 	if (it == vertices.end()) {
    // 	  print("new!");
    // 	  vertices.push_back(v);
    // 	  return vertices.end() - 1;
    // 	} else {
    // 	  print("not new");
    // 	  return it;
    // 	}
    //   }
      
    //   void depth_first_search(Vertex_ptr start) {
    //   	std::vector<Vertex_ptr> stack;
    //   	Label_t label = (*start).label;
    //   	stack.push_back(start);
		
    //   	while (not stack.empty()) {
    //   	  Vertex_ptr v = stack.back();
    // 	  stack.pop_back();
	  
    // 	  for (Vertex_ptr ptr : adj_list.at(v)) {
    // 	    if ((*ptr).label != label) {
    //   	      (*ptr).label = label;
    //   	      stack.push_back(ptr);
    //   	    }
    //   	  }   
    //   	}

    //   }
      
    //   std::vector<Label_t> connected_components() {
    // 	std::vector<Label_t> labels;
    // 	Label_t label;
    // 	Vertex_ptr vertex_ptr;
	
    // 	for(auto& pair : adj_list) {
    // 	  vertex_ptr = pair.first;
    // 	  label = (*vertex_ptr).label;
    // 	  if (label == (*vertex_ptr).original_label) { // found a new component
    // 	    depth_first_search(vertex_ptr);
    // 	    labels.push_back(label);
    // 	  }
    // 	}
    // 	return labels;
    //   }
            
    // };
       
    
    template <bool directed, class T, class Label_t=int>
    struct Graph {

      using Vertex_t = Vertex<T, Label_t>;
      using Vertices_t = std::set<Vertex_t*>;
      using Edge_t = Edge<T, Label_t>;
      using Edges_t = std::set<Edge_t>;
      using AdjList = std::map<Vertex_t*, Vertices_t>;
      AdjList adj_list;

      Graph() = default;
      Graph(const AdjList& list) : adj_list(list) {}

      static Vertices_t edges_to_vertices(const Edges_t& edges) {
    	Vertices_t out;
    	for(auto& e : edges) {
    	  out.insert(&(e.first));
    	  out.insert(&(e.second));
    	}
    	return out;
      }

      static Vertices_t adj_to_v(const Vertex_t* v, const Edges_t& edges) {
      	Vertices_t adj;
      	for (auto& e : edges) {
      	  if (&(e.first) == v)
      	    adj.insert(&(e.second));
    	  if constexpr (not directed) {
    	    if (&(e.second) == v)
    	      adj.insert(&(e.first));
    	  }
      	}
      	return adj;
      }

      static Graph from_edges(const Edges_t& edges) {
      	AdjList list;
      	Vertices_t all_v = edges_to_vertices(edges);

    	for(auto v : all_v)
    	  list.insert({v, adj_to_v(v, edges)});
       
      	return Graph(list);
      }

      void depth_first_search(Vertex_t* start) {
      	std::vector<Vertex_t *> stack;
      	Label_t label = start->label;
	
      	stack.push_back(start);
		
      	while (not stack.empty()) {
      	  auto v = stack.back();
    	  stack.pop_back();
	  
    	  auto adj = adj_list.at(v);
    	  for (auto ptr : adj) {
      	    if (ptr->label != label) {
      	      ptr->label = label;
      	      stack.push_back(ptr);
      	    }
      	  }   
      	}

      }
      
      std::vector<Label_t> connected_components() {
    	std::vector<Label_t> labels;
    	Label_t label;
    	for(auto& pair : adj_list) {
    	  Vertex_t* vertex_ptr = pair.first;
    	  label = vertex_ptr->label;
    	  if (label == vertex_ptr->original_label) { // found a new component
    	    depth_first_search(vertex_ptr);
    	    labels.push_back(label);
    	  }
    	}
    	return labels;
      }
      
      std::set<Vertex_t> vertices() const {
    	std::set<Vertex_t> out;
    	for (const auto& pair : adj_list)
    	  out.insert(*(pair.first));
    	return out;
      }
      
    };
    
  }
  
}
