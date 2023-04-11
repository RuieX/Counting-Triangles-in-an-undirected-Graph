// g++-6 -std=c++11 -O3 pi-future.cpp -o pi-future

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <numeric>
#include <future>

void PIworker(int id, int n, int skip, std::promise<double> sum) {
  double d = 1.0/(double)n;
  double s = 0.0;
  double x = 0.0;
  for (int i=id+1; i<=n; i+=skip) {
    x = (i-0.5)*d;
    s += 4.0/(1.0+x*x);
  }
  sum.set_value( d*s );
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
  std::vector<std::future<double>> partial_pi(NUM_THREADS);

  std::cout << "Starting..." << std::endl;
  auto start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < NUM_THREADS; i++) {
    std::promise<double> promise;
    partial_pi[i] = promise.get_future();
    threads[i] = std::thread(PIworker, i, STEPS, NUM_THREADS, std::move(promise) );
  }

  std::cout << "Waiting..." << std::endl;
  double pi = 0;
  for (int i=0; i<NUM_THREADS; i++)
    pi += partial_pi[i].get();

  for (int i=0; i<NUM_THREADS; i++)
    threads[i].join();

  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds> (end - start);

  std::cout << "     PI: " << pi << std::endl;
  std::cout << "Elapsed: " << elapsed.count() << " ms." << std::endl;

  return 0;
}

