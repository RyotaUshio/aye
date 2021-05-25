#pragma once

#include <aye/core.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <regex>

namespace aye {

  template <class Array>
  Array normalize(const Array& image, typename Array::dtype upper_bound=255) {
    auto max = numpy::max(image);
    auto min = numpy::min(image);
    auto out = image.copy();
    out -= min;
    out *= upper_bound / (max - min);
    return out;
  }

  template <class Array, class... Args>
  void savetxt(const char* fname, const Array& image, Args... args) {
    numpy::savetxt(fname, normalize(image),
		   " ", // delimiter
		   " ", // newline
		   python::str(image.shape(0)) + " " + python::str(image.shape(1)) + "\n", // header
		   "" // footer
		   );
  }

  template <class... Args>
  Image loadtxt(const char* fname, Args... args) {
    return numpy::loadtxt(fname, args...);
  }

  // template <class... Args>
  // Image imread(int argc, const char** argv, int argc_required, Args... args) {
  //   if (argc < argc_required)
  //     throw std::runtime_error(python::str(argc_required) +
  // 			       " arguments are required, got " +
  // 			       python::str(argc));
  //   return loadtxt(argv[1], args...);
  // }
  
  bool endswith(const std::string& s, const std::string& suffix) {
    // https://marycore.jp/prog/cpp/ends-with-has-suffix-search/
    if (s.size() < suffix.size())
      return false;
    return std::equal(std::rbegin(suffix), std::rend(suffix), std::rbegin(s));
  }

  std::string replace(const std::string& s, const std::string& old, const std::string& new_) {
    return std::regex_replace(s, std::regex(old), new_);
  }

  template <template <class...> class Container, class... Args>
  std::string join(const std::string& s, const Container<std::string, Args...>& iterable) {
    std::string ret = "";
    bool first = true;
    for (const auto& e : iterable) {
      if (not first)
	ret += s;
      ret += e;
      first = false;
    }
    return ret;
  }

  std::string _imread_get_options(unsigned int upper_bound) {
    return "--upper-bound " + std::to_string(upper_bound);
  }

  std::string _imread_get_options(double scale=1.0) {
    return "--scale " + std::to_string(scale);
  }
  
  template <class... Args>
  Image imread(const char* filename, Args... args) {
    if (endswith(filename, ".txt"))
      return loadtxt(filename, args...);
    std::string tmpfile =  "__aye__imread__.txt";
    std::string options = _imread_get_options(args...);
    std::system(("./io to_txt " + std::string(filename) + " -o " + tmpfile + " " + options).c_str());
    auto ret = loadtxt(tmpfile.c_str());
    std::system(("rm " + tmpfile).c_str());
    return ret;
  }

  void _imshow_impl_make_files(std::vector<std::string>& filenames) {}
  
  template <class Array, class... Rest>
  void _imshow_impl_make_files(std::vector<std::string>& filenames,
			       const Array& image,
			       const std::string& title,
			       const Rest&... rest) {
    std::string filename = "aye::imshow_" + replace(title, " ", "_");
    if (std::find(filenames.begin(), filenames.end(), filename) != filenames.end())
      throw std::runtime_error("imshow: Duplicated filenames");
    filenames.push_back(filename);
    savetxt<Array>(filename.c_str(), image);
    _imshow_impl_make_files(filenames, rest...);
  }

  void _imshow_impl_body(const std::vector<std::string>& filenames) {
    std::string fnames = join(" ", filenames);
    std::system(("./io imshow " + fnames).c_str());
    std::system(("rm " + fnames).c_str());
  }
  
  template <class Array, class... Rest>
  void imshow(const Array& image, const std::string& title, const Rest&... rest) {
    std::vector<std::string> filenames;
    _imshow_impl_make_files(filenames, image, title, rest...);
    _imshow_impl_body(filenames);
  }

  template <class Array, class... Args>
  void savefig(const std::string& fname, const Array& image, Args... args) {
    std::string tmpfile = "__aye_savefig__";
    savetxt<Array>(tmpfile.c_str(), image);
    std::system(("./io to_image " + tmpfile + " -o " + fname).c_str());
    std::system(("rm " + tmpfile).c_str());
  }
  
}
