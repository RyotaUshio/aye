#pragma once

#include <tuple> // forward_as_tuple

namespace aye {

  struct Harris {
    const Image& input;
    const Kernel& window;
    numpy::float64 sigma; // はじめの平滑化のガウスフィルタ
    numpy::float64 k;

    /* results */
    Image response; // distribution of the corner response function
    // Image corner; // local maximum of response

    Harris(const Image& input_, const Kernel& window_, numpy::float64 sigma_, numpy::float64 k_=0.05)
      : input(input_), window(window_), sigma(sigma_), k(k_) {
      set_response();
      // set_corner();
    }
    
  private:
    void set_response() {
      auto I_x = convolve(input, Sobel_x);
      auto I_y = convolve(input, Sobel_y);
      auto I_xx = I_x * I_x;
      auto I_xy = I_x * I_y;
      auto I_yy = I_y * I_y;
      auto S_xx = convolve(I_xx, window);
      auto S_xy = convolve(I_xy, window);
      auto S_yy = convolve(I_yy, window);

      auto det = S_xx * S_yy - S_xy * S_xy;
      auto trace = S_xx + S_yy;
      response = det - k * trace * trace;
    }

    // non-maximum supression
    // void set_corner() {
    
    // }
    
  };

  
  struct CannyThreshold {
    
    using Vertex = algorithm::Vertex<int, Binary>;
    using Vertices = std::map<np::coord_type, Vertex>;
    using Edge = algorithm::Edge<int, Binary>;
    using Edges = std::set<Edge>;
    using Graph = algorithm::UndirectedGraph<int, Binary>;

    Vertices vertices;
    Graph graph;
    std::vector<Vertex*> strong_edge_vertices;

    BinImage may_be_edge;
    BinImage strong_edge;
    BinImage output;
    
    CannyThreshold() = default;
    CannyThreshold(const Image& local_max, np::float_ th_low, np::float_ th_high)
      : may_be_edge( np::empty<Binary>(local_max.shape()) ),
	strong_edge( np::empty<Binary>(local_max.shape()) ) {
      
      auto it_max = local_max.begin();
      auto it_may_be_edge = may_be_edge.begin();
      auto it_strong_edge = strong_edge.begin();
      auto end = local_max.end();

      for(; it_max != end; it_max++, it_may_be_edge++, it_strong_edge++) {
	*it_may_be_edge = (*it_max > th_low) ? 1 : 0;
	*it_strong_edge = (*it_max > th_high) ? 1 : 0;
      }
      
      make_graph(may_be_edge, strong_edge);
      
      for(Vertex* strong_edge_pixel : strong_edge_vertices)
	graph.depth_first_search(strong_edge_pixel);

      output = may_be_edge.copy();
      make_output();
    }

    void make_vertices(const BinImage& may_be_edge, const BinImage& strong_edge) {
      auto shape = may_be_edge.shape();
      auto M = shape[0];
      auto N = shape[1];
      int key = 0;
      
      for (int i=0; i<M; i++)
	for (int j=0; j<N; j++) {
	  if (may_be_edge[{i, j}]) { // pixel value > th_low
	    bool is_strong = strong_edge[{i, j}];
	    Label label = is_strong ? 1 : 0;
	    np::coord_type coord{i, j};
	    vertices.emplace(coord,			     
			     Vertex(key++, label));
	    if (is_strong) {
	      Vertex* ptr = &(vertices.at({i, j}));
	      strong_edge_vertices.push_back(ptr);
	    }
	  }
	}
    }

    Graph::AdjList make_adj_list() {
      Graph::AdjList list;
      
      for(const auto& k_v : vertices) {
	int i = k_v.first[0];
	int j = k_v.first[1];
	const Vertex& v = k_v.second;
	for (int m=-1; m<=1; m++)
	  for (int n=-1; n<=1; n++) {
	    if (m == n == 0)
	      continue;
	    try {
	      (list[const_cast<Vertex*>(&v)]).insert(&(vertices.at({i+m, j+n})));
	    } catch (const std::out_of_range& e) {
	      continue;
	    }
	  }
      }
      return list;
    }

    void make_graph(const BinImage& may_be_edge, const BinImage& strong_edge) {
      make_vertices(may_be_edge, strong_edge);
      auto list = make_adj_list();
      graph = Graph(list);
    }
    
    void make_output() {
      int i, j;
      for(i=0; i<output.shape(0); i++)
	for(j=0; j<output.shape(1); j++)
	  if (output[{i, j}]) {
	    output[{i, j}] = vertices.at({i, j}).label;
	  }
    }
  };
  
  
  struct Canny {
    
    Gradient* grad;
    Image local_max;
    CannyThreshold threshold_result;
    
    Canny(const Image& input,
	  numpy::float64 sigma,
	  numpy::float64 th_low,
	  numpy::float64 th_high,
	  int order=2) {
      
      // denoising & computing the gradient
      grad = new Sobel(convolve(input, Gauss(sigma)), order);
      // detect local maximum of the magnitude of gradient
      non_maximum_suppression();
      // hysterisis thresholding
      threshold_result = CannyThreshold(local_max, th_low, th_high);
    }

    void non_maximum_suppression() {
      auto tan = grad->diff_y / grad->diff_x;
      numpy::float64 t;
      numpy::float64 next1, next2;
      local_max = apply_local(grad->magnitude, 1,
			    [&](int i, int j, const Image& local){
			      t = tan[{i, j}];
			      np::float_ target = local[4];
			      
			      if (t >= 1) {
				next1 = local[1] * (1.0 - 1.0/t) + local[2] / t;
				next2 = local[7] * (1.0 - 1.0/t) + local[6] / t;
			      } else if (t <= -1) {
				next1 = local[1] * (1.0 + 1.0/t) - local[0] / t;
				next2 = local[7] * (1.0 + 1.0/t) - local[8] / t;
			      } else if (0 <= t and t < 1) {
				next1 = local[5] * (1.0 - t) + local[2] * t;
				next2 = local[3] * (1.0 - t) + local[6] * t;
			      } else if (-1 < t and t < 0) {
				next1 = local[5] * (1.0 + t) - local[8] * t;
				next2 = local[3] * (1.0 + t) - local[0] * t;				
			      }
			      return (next1 <= target and next2 <= target) ? target : 0.0;
			    });
   
      local_max = normalize(local_max);
    }
    
    ~Canny() {
      delete grad;
    }

    BinImage result() {
      return threshold_result.output;
    }    
  };
  
}
