// g++ -std=c++11 -O3 pi-std-threads.cc -o pi-std-threads

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <numeric>

void PIworker(int id, int n, int skip, double* result) {
  double d = 1.0/(double)n;
  double s = 0.0;
  double x = 0.0;
  for (int i=id+1; i<=n; i+=skip) {
    x = (i-0.5)*d;
    s += 4.0/(1.0+x*x);
  }
  *result = d*s;
}

int main(int argc, char **argv) {
  if (argc!=3) {
    std::cout << " Usage: " << argv[0] << " <num_threads> <num_steps>" << std::endl;
    std::exit(EXIT_FAILURE);
  }


  std::cout << "No. of available cores : " << std::thread::hardware_concurrency()
            << std::endl;

  const int NUM_THREADS = atoi(argv[1]);
  const int STEPS = atoi(argv[2]);


  std::vector<std::thread> threads(NUM_THREADS);
  std::vector<double> partial_pi(NUM_THREADS);

  std::cout << "Starting..." << std::endl;
  auto start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < NUM_THREADS; i++) {
    threads[i] = std::thread(PIworker, i, STEPS, NUM_THREADS, &partial_pi[i]);
  }

  std::cout << "Waiting..." << std::endl;
  for (int i = 0; i < NUM_THREADS; i++)
    threads[i].join();

  double pi = std::accumulate(partial_pi.begin(), partial_pi.end(), 0.0);

  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds> (end - start);

  std::cout << "     PI: " << pi << std::endl;
  std::cout << "Elapsed: " << elapsed.count() << " ms." << std::endl;

  return 0;
}


