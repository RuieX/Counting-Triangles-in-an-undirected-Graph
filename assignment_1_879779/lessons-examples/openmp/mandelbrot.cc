#include <complex>
#include <cstdio>
#include <iostream>
#include <chrono>
#include <vector>
#include <cmath>

// g++-6 -std=c++11 -fopenmp -O3 mandelbrot.cc -o mandelbrot

int MandelbrotCalculate(std::complex<double> c, int maxiter)
{
  // iterates z = z^2 + c until |z| >= 2 or maxiter is reached,
  // returns the number of iterations.
  std::complex<double> z = c;
  int n=0;
  for(; n<maxiter; ++n)
  {
    if( std::abs(z) >= 2.0) break;
    z = z*z + c;
  }
  return n;
}

void mandelbrot(int maxiter, int n_threads)
{
  std::cout << "Starting..." << std::endl;
  auto time_start = std::chrono::high_resolution_clock::now();


  const int width = 120, height = 30, num_pixels = width*height;

  const std::complex<double> center(-.7, .7), span(2.7, -(4/3.0)*2.7*height/width);
  const std::complex<double> begin = center-span/2.0, end = center+span/2.0;

  std::vector<int> image (num_pixels);


// basic, not working very well
// #pragma omp parallel for num_threads(n_threads) 
// #pragma omp parallel for schedule(static, 16) num_threads(n_threads) 
// much better
// #pragma omp parallel for schedule(dynamic, 100) num_threads(n_threads) 
// below the best
#pragma omp parallel for schedule(dynamic, 1) num_threads(n_threads) 
// reasonably ok
// #pragma omp parallel for schedule(dynamic, 4) num_threads(n_threads) 
// not as well as expected
// #pragma omp parallel for schedule(guided, 1) num_threads(n_threads) 
// #pragma omp parallel for schedule(guided, 16) num_threads(n_threads) 
  for(int pix=0; pix<num_pixels; ++pix) {
    const int x = pix%width, y = pix/width;

    std::complex<double> c = begin + std::complex<double>(x * span.real() / (width +1.0),
                                                          y * span.imag() / (height+1.0));

    int n = MandelbrotCalculate(c, maxiter);
    if(n == maxiter) n = 0;

    image[pix] = n;
  }

  auto time_end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start);


  static const char charset[] = ".,rjcopgawJOQPGE8M@";
  for(int pix=0; pix<num_pixels; ++pix) {
  	if (image[pix]>0)
  		std::cout << charset[image[pix] % (sizeof(charset)-1)];
  	else
  		std::cout << " ";

     if( pix%width+1 == width) std::cout << std::endl;
  }


  std::cout << "elapsed time : "<< elapsed.count() << " ms." << std::endl;
}



int main (int argc, char* argv[]) {

  if (argc!=3) {
    std::cout << " !! Error !! " << std::endl;
    std::cout << " !! Usage: " << std::endl;
    std::cout << "\t ./mandelbrot <max_iter> <num_threads>" << std::endl;
    return 1;
  }

  int max_iter = atoi(argv[1]);
  int num_threads = atoi(argv[2]);

  mandelbrot(max_iter, num_threads);

  return 0;
}

