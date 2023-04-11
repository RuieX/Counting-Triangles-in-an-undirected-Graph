/*
 * pi-std-threads.cc
 *
 */

// g++ -std=c++11 -O3 -lpthread pi-std-future.cc -o pi-std-future-10

#include <iostream>
#include <chrono>
#include <thread>
#include <future>

#define STEPS 1000000000
#define NUM_THREADS 10

void PIworker(int id, int n, int skip, std::promise<double> sum) {
  double d = 1.0/(double)n;
  double s = 0.0, x;
  for (int i=id+1; i<=n; i+=skip) {
    x = (i-0.5)*d;
    s += 4.0/(1.0+x*x);
  }
  sum.set_value( d*s );
}

int main(int argc, char **argv) {
  std::cout << "No. of available cores : " << std::thread::hardware_concurrency()
            << std::endl;


  std::thread threads[NUM_THREADS];
  std::future<double> future[NUM_THREADS];

  std::cout << "Starting..." << std::endl;
  auto start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < NUM_THREADS; i++) {
    std::promise<double> promise;
    future[i] = promise.get_future();
    threads[i] = std::thread(PIworker, i, STEPS, NUM_THREADS, std::move(promise) );
  }

  double pi = 0;
  for (int i=0; i<NUM_THREADS; i++)
    pi += future[i].get();

  std::cout << "waiting..." << std::endl;
  for (int i = 0; i < NUM_THREADS; i++)
    threads[i].join();

  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds> (end - start);

  std::cout << "     PI: " << pi << std::endl;
  std::cout << "Elapsed: " << elapsed.count() << " ms." << std::endl;

  return 0;
}



