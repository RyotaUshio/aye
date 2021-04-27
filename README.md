# Eyeball

Image processing & computer vision library for C++, based on [NumPy-in-C++](https://github.com/RyotaUshio/numpy-in-cpp.git).

## Requirements

- C++17 or later
- [NumPy-in-C++](https://github.com/RyotaUshio/numpy-in-cpp.git)

## Contents

- Convolutional filtering
- Fast fourier transform (FFT) and frequency filtering
- Sparse representation with the discrete cosine transform (DCT)
- Feature detection & description
  - Harris's corner detection
  - Canny's edge detection
  - (SIFT: coming soon!)
- Binary image processing
  - Connected components labeling
- (Computational photography & image restoration: coming soon!)
- Basic algorithms & data structures for general purposes
  - Graphs
- (Machine learning models: coming soon!)
- File I/O powered by Python
  - C++ functions for read/write (`savetxt()`, `loadtxt()`, `savefig()`) and displaying (`imshow()`)
  - command line tools (`io to_txt`, `io to_image`, `io imshow`)