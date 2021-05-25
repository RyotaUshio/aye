#pragma once

#include <tuple> // forward_as_tuple
#include <list>

namespace aye {

  using python::print;

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

  struct DoG;
  
  struct DoG_keypoint {
    const DoG* DoG_result;
    const int x;
    const int y;
    const int octave;
    const int i_scale;

    DoG_keypoint(const DoG* DoG_result_,
		 int x_,
		 int y_,
		 int octave_,
		 int i_scale_)
      : DoG_result(DoG_result_), x(x_), y(y_), octave(octave_), i_scale(i_scale_) {}

    np::float_ get_scale() const;
    np::float_ get_value() const;
    Image get_smoothed_image() const;
    Image get_DoG_image() const;
    bool has_low_constrast(np::float_) const;
    bool is_edge() const;
    std::vector<np::float_> make_histogram(const std::vector<std::vector<CenteredDiff>>& gradients) const;
  };

  struct DoG {

    const np::float_ sigma;
    const int s;
    const np::float_ k;
    const int min_size;
    const int n_scale_per_octave;

    using StackedImages = np::ndarray<np::float_>;
    std::vector<StackedImages> scale_space; // Each element represents an octave, which consists of n_scale_per_octave images.
    std::vector<StackedImages> result;
    std::list<DoG_keypoint> extrema;
    
    DoG(const Image& input,
	int s_=3, 
	int min_size_=10,
	np::float_ sigma_=1.6)
      : sigma(sigma_), s(s_),
	k(std::pow(2, 1.0/s)),
	min_size(min_size_),
	n_scale_per_octave(s + 3) {
      build_scale_space(input);
      subtract_gaussians();
      detect_extrema();
      eliminate_low_quality(input);
    }

    void build_scale_space(const Image& input) {
      Image input_subsampled = input;
      np::float_ scale;
  
      using python::range;
      using python::slice;

      while (input_subsampled.shape(0) > min_size) {
	scale_space.push_back(np::empty({n_scale_per_octave,
					 input_subsampled.shape(0),
					 input_subsampled.shape(1)}));
	scale = sigma;
	for (auto i_scale : range(n_scale_per_octave)) {
	  scale_space.back()(i_scale) = convolve(input_subsampled, Gauss(scale, 2.0));
	  scale *= k;
	}
	const auto& octave_above = scale_space.back()(s);
	input_subsampled = octave_above("::2", "::2");
      }
    }

    void subtract_gaussians() {
      using python::range;

      auto n_octave = scale_space.size();
      for (auto octave : range(n_octave)) {
	auto shape = scale_space[octave].shape();
	result.push_back(np::empty({n_scale_per_octave - 1, shape[1], shape[2]}));
	result.back() = scale_space[octave](":-1") - scale_space[octave]("1:");
      }
    }

    bool is_extrema(np::float_ center, const np::ndarray<np::float_>& neighbors) {
      auto first = *neighbors.begin();
      int i = 0;
      // i == 13 is the center pixel of the neighborhood

      if (center == first)
      	return false;

      if (center > first) {
      	for (const auto e : neighbors) {
      	  if (i != 13 and center <= e)
      	    return false;
      	  i++;
      	}
      } else {
      	for (const auto e : neighbors) {
      	  if (i != 13 and center >= e)
      	    return false;
      	  i++;
      	}
      }
      return true;
    }
    
    void detect_extrema() {
      using python::range;
      using python::slice;

      auto n_octave = scale_space.size();
      for (auto octave : range(n_octave)) {
	auto shape = result[octave].shape();
	for (auto i_scale : range(1, shape[0] - 1))
	  for (auto i : range(1, shape[1] - 1))
	    for (auto j : range(1, shape[2] - 1)) {
	      auto center = result[octave][{i_scale, i, j}];
	      auto neighbors = result[octave](slice(i_scale-1, i_scale+2),
					      slice(i-1, i+2),
					      slice(j-1, j+2));
	      if (is_extrema(center, neighbors)) {
		DoG_keypoint keypoint{this, i, j, octave, i_scale};
		extrema.push_back(keypoint);
	      }
	    }
      }
    }

    void eliminate_low_quality(const Image& input) {
      int count = 0;

      np::float_ max, min, th;
      max = np::max(input);
      min = np::min(input);
      th = 0.03;// * (max - min) + min;
      
      for (auto it=extrema.begin(); it!=extrema.end(); it++) {
	if ((*it).has_low_constrast(th) or 
	    (*it).is_edge()) {
	  extrema.erase(it);
	  count++;
	}
      }
      python::print("deleted", count, "keypoints");
    }

    void show_scale_space() const {
      using python::str;
      using python::range;
      
      for (auto octave : range(scale_space.size()))
	for (auto i_scale : range(n_scale_per_octave))
	  imshow(scale_space[octave](i_scale), "scale_space " + str(octave) + "-" + str(i_scale));
    }
    
    void show_result() const {
      using python::str;
      using python::range;
      
      for (auto octave : range(scale_space.size()))
	for (auto i_scale : range(n_scale_per_octave - 1))
	  imshow(result[octave](i_scale), "DoG " + str(octave) + "-" + str(i_scale));
    }

    void save_extrema(const char* fname) const {
      std::ofstream ofs(fname);
      if (!ofs)
    	throw std::runtime_error("cannot open file " + std::string(fname));
      for (const auto& keypoint : extrema)
    	ofs << keypoint.get_scale() << " "
    	    << keypoint.x << " "
    	    << keypoint.y << "\n";
    }
    
  };

  np::float_ DoG_keypoint::get_scale() const {
    return DoG_result->sigma * std::pow(2.0, octave) * std::pow(DoG_result->k, i_scale);
  }

  np::float_ DoG_keypoint::get_value() const {
    return DoG_result->result[octave][{i_scale, x, y}];
  }

  Image DoG_keypoint::get_smoothed_image() const {
    return DoG_result->scale_space[octave](i_scale);
  }
  
  Image DoG_keypoint::get_DoG_image() const {
    return DoG_result->result[octave](i_scale);
  }
  
  bool DoG_keypoint::has_low_constrast(np::float_ threshold) const {
    // threshold should be a value in [0, 1].
    const auto& image = get_DoG_image();
    np::float_ max = np::max(image);
    np::float_ min = np::min(image);
    np::float_ val = get_value();
    np::float_ normalized = (val - min) / (max - min);
    // return val < threshold * (max - min) + min;
    // python::print("normalized =", normalized);
    return (normalized < threshold);
    // return val < threshold;
  }

  bool DoG_keypoint::is_edge() const {
    auto D = get_DoG_image();
    auto Dxx = 2 * D[{x, y}] - D[{x - 1, y}] - D[{x + 1, y}];
    auto Dyy = 2 * D[{x, y}] - D[{x, y - 1}] - D[{x, y + 1}];
    auto Dxy = (D[{x + 1, y + 1}] - D[{x + 1, y - 1}] - D[{x - 1, y + 1}] + D[{x - 1, y - 1}]) / 4.0;
    
    auto trace = Dxx + Dyy;
    auto det = Dxx * Dyy - Dxy * Dxy;
    auto val = trace * trace / det;
    auto th = 10;
    return val > (th + 1) * (th + 1) / th;
  }

  std::vector<np::float_> DoG_keypoint::make_histogram(const std::vector<std::vector<CenteredDiff>>& gradients) const {
    using python::slice;
    using python::print;

    auto grad = gradients[octave][i_scale];
    auto scale = DoG_result->sigma * std::pow(DoG_result->k, i_scale);
    auto weight = Gauss(1.5 * scale, 2.0);
    int size = weight.shape(0) / 2;
    auto slice_x = slice(x, x + 2*size + 1);
    auto slice_y = slice(y, y + 2*size + 1);
    auto mag = pad_nearest(grad.magnitude, size)(slice_x, slice_y);
    auto angle = pad_nearest(grad.direction, size)(slice_x, slice_y);
    weight *= mag;
    
    std::vector<np::float_> hist(36, 0);
      
    auto it_weight = weight.begin();
    auto it_angle = angle.begin();
    auto end = angle.end();
    while(it_angle != end) {
      int quantized = ((int)((*it_angle / np::pi + 1.0) * 18.0)) % 36; // quantized angle (36 bins)
      hist[quantized] += *it_weight;
      it_weight++;
      it_angle++;
    }

    return hist;
  }
 
  struct SIFT;
  
  struct SIFT_keypoint : public DoG_keypoint {

    const SIFT* SIFT_result;
    np::float_ orientation;
    np::ndarray<np::float_> sift;

    SIFT_keypoint(const SIFT* SIFT_result_, const DoG_keypoint& key, np::float_ orient)
      : DoG_keypoint(key.DoG_result, key.x, key.y, key.octave, key.i_scale),
    	SIFT_result(SIFT_result_), 
    	orientation(orient) {
      calc_sift();
    }
    
    // SIFT_keypoint(const SIFT* SIFT_result_, const DoG_keypoint& key, np::float_ orient)
    //   : DoG_keypoint(key.DoG_result, key.x, key.y, key.octave, key.i_scale),
    // 	SIFT_result(SIFT_result_), 
    // 	orientation(orient) {
    //   calc_sift();
    // }

    void calc_sift();

    std::vector<np::float_> make_histogram(const Image& mag4x4, const Image& angle4x4) const;
    
    static np::float_ get_orientation(const std::vector<np::float_>& hist, int index) {
      auto a = hist[(index - 1 + 36) % 36];
      auto b = hist[index];
      auto c = hist[(index + 1) % 36];
      auto delta = (a - c) / (2.0 * a - 4.0 * b + 2.0 * c); // parabola-fitted displacement
      auto angle = (index + delta - 18.0) / 18.0 * np::pi;
      return angle; // -pi -- pi
    }

    static Image Gaussian_window() {
      np::float_ sigma = 8;
      np::size_type size = 16; // size of the kernel
    
      auto xy = np::indices<np::float64>({size, size}) - sigma;
      auto x = xy(0);
      auto y = xy(1);
      auto x2_y2 = x*x + y*y;
    
      auto out = np::exp(-x2_y2 / (2.0 * sigma * sigma));
      return out;
    }
    
  };


  
  struct SIFT {

    const DoG DoG_result;
    std::vector<std::vector<CenteredDiff>> gradients;
    std::vector<SIFT_keypoint> keypoints;

    SIFT(const Image& input,
	 int s_=3, 
	 int min_size_=10,
	 np::float_ sigma_=1.6)
      : DoG_result(input, s_, min_size_, sigma_) {
      set_gradients();

      int i=0;
      for (auto& keypoint : DoG_result.extrema) {
	add_keypoints(keypoint);
	// print(i, "/", DoG_result.extrema.size()); i++;
      }
    }

    void set_gradients() {
      for (const auto& smoothed_images : DoG_result.scale_space) {
	// smoothed_images : a stack of smoothed images within an octave
	std::vector<CenteredDiff> gradients_in_octave;
	for (auto i_scale : python::range(DoG_result.n_scale_per_octave)) {
	  gradients_in_octave.emplace_back(smoothed_images(i_scale));
	}
	gradients.push_back(gradients_in_octave);
      }
    }
    
    void add_keypoints(const DoG_keypoint& key){
      /* generate one or more SIFT_keypoint(s) from a DoG_keypoint & add them to `keypoints` */

      //print("1");
      
      auto hist = key.make_histogram(gradients);

      //print("2");
      
      auto max_it = std::max_element(hist.begin(), hist.end());
      auto max = *max_it;

      // print("3");
    
      for (int index=0; index<hist.size(); index++) {
	
	//print("A");
	
	if (hist[index] >= max * 0.8) {
	  // print("B");
	  auto orient = SIFT_keypoint::get_orientation(hist, index);
	  // print("C");
	  keypoints.emplace_back(this, key, orient);
	  // print("D");
	}

	// print("E");
      }

      // print("4");
    }

    void save(const char* fname) const {
      std::ofstream ofs(fname);
      if (!ofs)
    	throw std::runtime_error("cannot open file " + std::string(fname));
      for (const auto& keypoint : keypoints)
    	ofs << keypoint.x << " "
    	    << keypoint.y << " "
	    << keypoint.get_scale() << " "
	    << keypoint.orientation << " "
	    << keypoint.sift.__repr__() << "\n";
    }

    static void two_smallest(const std::vector<np::float_>& vec,
			     std::size_t& first, std::size_t& second) {
      auto it_first = std::min_element(vec.begin(), vec.end());
      auto it_second1 = std::min_element(vec.begin(), it_first);
      auto it_second2 = std::min_element(it_first + 1, vec.end());
      auto begin = vec.begin();
      first = it_first - begin;
      second = (*it_second1 < *it_second2) ? it_second1 - begin : it_second2 - begin;
    }
    
    static void match(const SIFT& sift1, const SIFT& sift2, np::float_ k, const char* fname) {
      std::vector<std::vector<SIFT_keypoint>> matches;
      
      for (const auto& key1 : sift1.keypoints) {
	std::vector<np::float_> dists;
	for (const auto& key2 : sift2.keypoints) {
	  auto diff = key1.sift - key2.sift;
	  auto dist_sq = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0);
	  dists.push_back(dist_sq);
	}
	std::size_t first, second;
	two_smallest(dists, first, second);
	if (dists[first] / dists[second] < k) {
	  matches.push_back({key1, sift2.keypoints[first]});
	}
      }

      std::ofstream ofs(fname);
      if (!ofs)
    	throw std::runtime_error("cannot open file " + std::string(fname));
      for (const auto& m : matches)
    	ofs << m[0].x << " "
    	    << m[0].y << " "
	    << m[1].x << " "
	    << m[1].y << "\n";
    }
    
  };

  void SIFT_keypoint::calc_sift() {
    int max_size = 16;
    const auto& grad = SIFT_result->gradients[octave][i_scale];
    auto mag = rotate2d(grad.magnitude,
			x, y,
			x - max_size, y - max_size,
			x + max_size, y + max_size,
			-orientation);
    auto angle = rotate2d(grad.direction,
			  x, y,
			  x - max_size, y - max_size,
			  x + max_size, y + max_size,
			  -orientation);
    python::slice s(8, 24);
    auto weight16x16 = mag(s, s) * Gaussian_window();
    auto angle16x16 = np::fmod(angle(s, s) - orientation + 4.0 * np::pi, 2.0 * np::pi);
    // each elem. of angle16x16 is in [0, 2*pi)

    using python::slice;

    int i, j;
    std::vector<np::float_> hist;
    for (i=0; i<4; i++)
      for (j=0; j<4; j++) {
	slice s1(4*i, 4*(i+1));
	slice s2(4*j, 4*(j+1));
	auto hist_local = make_histogram(weight16x16(s1, s2), angle16x16(s1, s2));
	std::copy(hist_local.begin(), hist_local.end(), std::back_inserter(hist));
      }
    	
    sift = np::ndarray<np::float_>(hist, {128});
  }

    
  std::vector<np::float_> SIFT_keypoint::make_histogram(const Image& weight4x4, const Image& angle4x4) const {
    using python::slice;
    using python::print;
    
    std::vector<np::float_> hist(8, 0);
      
    auto it_weight = weight4x4.begin();
    auto it_angle = angle4x4.begin();
    auto end = angle4x4.end();
    while(it_angle != end) {
      int quantized = ((int)(*it_angle / np::pi * 4.0)) % 8; // quantized angle (8 bins)
      hist[quantized] += *it_weight;
      it_weight++;
      it_angle++;
    }

    return hist;
  }

  
}
