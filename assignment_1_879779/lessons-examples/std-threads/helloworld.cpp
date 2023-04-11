// g++-6 -std=c++11 -O3 helloworld.cpp -o helloworld

#include <thread>
#include <iostream>
#include <vector>

void hello(void) {
  std::cout << "Hello World!" << std::endl;
}

int main(int argc, char **argv) {
  if (argc!=2) {
    std::cout << " Usage: " << argv[0] << " <num_threads>" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  const int NUM_THREADS = atoi(argv[1]);

  std::vector<std::thread> threads(NUM_THREADS);

  for (int i = 0; i < NUM_THREADS; i++) 
    threads[i] = std::thread(hello);

  for (int i = 0; i < NUM_THREADS; i++)
    threads[i].join();
}
