#include <cstdio>
#include <iostream>
#include <chrono>

// g++ -std=c++11 -fopenmp -O3 openmp-pi.cc -o openmp-pi
// srun -v -I -c 39 ./openmp-pi 1000000000 32

void computePi(int n_points, int n_threads) {
  std::cout << "Starting..." << std::endl;
  auto start = std::chrono::high_resolution_clock::now();

  double d = 1.0/(double)n_points;
  double pi = 0.0;
  double x = 0.0;

  #pragma omp parallel for num_threads(n_threads) reduction(+:pi) \
    private(x) firstprivate(d)
  for (int i=1; i<=n_points; i++) {
    x = (i-0.5)*d;
    pi += 4.0/(1.0+x*x);
  }

  pi *= d;

  auto end = std::chrono::high_resolution_clock::now();

  auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);

  std::cout << "PI = " << pi << std::endl;
  std::cout << "elapsed time : "<< elapsed.count() << " secs." << std::endl;
}

int main (int argc, char* argv[]) {

  if (argc!=3) {
    std::cout << " !! Error !! " << std::endl;
    std::cout << " !! Usage: " << std::endl;
    std::cout << "\t ./openmp-pi <num_samples> <num_threads>" << std::endl;
    return 1;
  }

  int num_samples = atoi(argv[1]);
  int num_threads = atoi(argv[2]);

  computePi(num_samples, num_threads);

  return 0;
}

