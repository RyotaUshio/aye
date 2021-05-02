#pragma once

#include <utility>
#include <vector>
#include <set>

namespace aye {

  using Label = int;
  using Binary = int;
  using BinImage = np::ndarray<Binary>;
  using LabelImage = np::ndarray<Label>;

  using python::print;

  // Connected-component labeling

  enum Connectivity {FOUR=4, EIGHT=8};
  template <Connectivity connectivity> struct ConnectedComponent;
  using ConnectedComponent4 = struct ConnectedComponent<FOUR>;
  using ConnectedComponent8 = struct ConnectedComponent<EIGHT>;

  namespace {
    using Vertex = algorithm::Vertex<Label, Label>;
    using Vertices = std::vector<Vertex>;
    using Edge = algorithm::Edge<Label, Label>;
    using Edges = std::set<Edge>;
    using Graph = algorithm::UndirectedGraph<Label, Label>;
  }
  
  template <Connectivity connectivity>
  struct ConnectedComponent {

    std::set< std::pair<Label, Label> > lookup;
    Vertices vertices;
    Graph graph;
    
    LabelImage label;
    int n_label;

    ConnectedComponent() = default;
    ConnectedComponent(const BinImage& input) {
      _set_temp_labels(input);
      _reset_labels();
    }

    BinImage::iterator _find_black(BinImage::iterator first, BinImage::iterator last) {
      /**
       * label
       *   0 : unlabeled
       *   1 : label 1
       *   2 : label 2
       * ...
       */
      auto itr = first;
      while(!*itr and itr != last) ++itr;
      return itr;
    }

    void _update_lookup(LabelImage::iterator target_label, Label another_label) {
      if (another_label and another_label != *target_label)
	lookup.insert(std::minmax(another_label, *target_label));
    }
    
    void _update_lookup(LabelImage::iterator target_label,
		       Label above_right, Label right) {
      if (above_right and above_right != *target_label and !right)
	lookup.insert(std::minmax(above_right, *target_label));
    }

    void _set_label_to_pixel_4(Label& new_label,
			       LabelImage::iterator target_label,
			       Label above, Label left) {
      if (above) {
	*target_label = above;
	_update_lookup(target_label, left);
      }
      else {
	if (left)
	  *target_label = left;
	else 
	  *target_label = new_label++;
      }
    }
    
    void _set_label_to_pixel_8(Label& new_label,
			       LabelImage::iterator target_label,
			       Label above_left,
			       Label above,
			       Label above_right,
			       Label left,
			       Label right
			       ) {
      if (above_left or above) {
	if (above_left) {
	  *target_label = above_left;
	  _update_lookup(target_label, above);
	  _update_lookup(target_label, above_right, right);
	} else {
	  *target_label = above;
	}
	_update_lookup(target_label, left);
	
      } else {
	if (left)
	  *target_label = left;
	else 
	  *target_label = new_label++;
      }      
    }

    void _set_temp_labels(const BinImage& input) {
      auto input_pad = pad_zero(input, 1);
      auto label_pad = np::zeros<Label>(input_pad.shape());
      auto itr = input_pad.begin() + (input_pad.shape(1) + 1);
      auto end = input_pad.end();
      auto label_begin = label_pad.begin();
      auto target_label = label_begin;
      Label new_label = 1;

      while ((itr = _find_black(itr, end)) != end) {
	
	auto coord = itr.get_coord();
	auto i = coord[0];
	auto j = coord[1];
	
	target_label = label_begin + itr.get_index();
	if constexpr (connectivity == FOUR) {
	  _set_label_to_pixel_4(new_label,
			      target_label,
			      label_pad[{i-1, j}],
			      label_pad[{i, j-1}]);
	  } else {
	  _set_label_to_pixel_8(new_label,
				target_label,
				label_pad[{i-1, j-1}],
				label_pad[{i-1, j  }],
				label_pad[{i-1, j+1}],
				label_pad[{i,   j-1}],
				label_pad[{i,   j+1}]);
	}
	itr++;
      }

      label = label_pad("1:-1", "1:-1");
      n_label = new_label - 1;
    }

    void _make_graph() {
      // make vertices
      for(Label i=1; i<=n_label; i++)
	vertices.push_back(Vertex(i, i));
      
      // make edges
      Edges E;
      for(const auto& pair : lookup)
	E.insert(Edge(vertices[pair.first - 1], vertices[pair.second - 1]));

      // make graph
      graph = Graph::from_edges(E);
      // graph = Graph(E);
    }

    void _reset_labels() {
      _make_graph();
      graph.connected_components();
      for(auto& old_label : label) {
	if (old_label > 0) {
	  auto new_label = vertices[old_label-1].label;
	  old_label = new_label;
	}
      } 
    }
        
  };
  
}
