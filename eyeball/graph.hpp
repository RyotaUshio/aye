#pragma once

#include <eyeball/eyeball.hpp>
#include <map>
#include <utility>
#include <initializer_list>

namespace eyeball {

  namespace algorithm {

    using python::print;
    using python::str;

    template <class T, class Label_t=int>
    struct Vertex {
      
      using type = T;
      using label_type = Label_t;
      
      T key;
      Label_t label;
      const Label_t original_label;
      
      Vertex(T val, Label_t lab) : key(val), label(lab), original_label(label) {}
      Vertex(T val) : Vertex(val, Label_t()) {}
      
      bool operator<(const Vertex<T, Label_t>& rhs) const {
	return key < rhs.key;
      }
      
      bool operator==(const Vertex<T, Label_t>& rhs) const {
	return key == rhs.key; // this == &rhs;
	
      }
      std::string __repr__() const {
	return "Vertex<" + python::str(typeid(T)) + ", " + python::str(typeid(Label_t))+ ">(" +
	  "key=" + python::str(key) + ", label=" + python::str(label) + ")";
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
	return "Edge<" + python::str(typeid(T)) + ", " + python::str(typeid(Label_t))+ ">(" +
	  "first=" + python::str(first) + ", second=" + python::str(second) + ")";
	  
      }
    };

    template <bool directed, class T, class Label_t> struct Graph;
    template <class T, class Label_t=int> using DirectedGraph = Graph<true, T, Label_t>;
    template <class T, class Label_t=int> using UndirectedGraph = Graph<false, T, Label_t>;
    
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
