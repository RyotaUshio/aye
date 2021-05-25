#pragma once

#include <aye/core.hpp>

namespace aye {

  struct Pyramid {
    
    Images images;

    Pyramid(const Image& image, std::size_t n) : images(n) {
      images[0] = image;
      _build();
    }

    void _build() {
      for(int i=0; i<images.size()-1; i++)
	images[i + 1] = pool(images[i]);
    }
  };
  
}
