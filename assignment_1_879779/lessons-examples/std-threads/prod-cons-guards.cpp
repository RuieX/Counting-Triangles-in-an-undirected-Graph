// g++ -std=c++11 -O3 prod-cons-guards.cpp -o prod-cons-guards

#include <iostream>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <cstdlib>    // random
#include <chrono>

std::queue<std::thread::id> shared_queue;
std::mutex shared_queue_mutex;

void producer(int time_msec, int runs) {
  std::thread::id my_id = std::this_thread::get_id();
  for (int i=0; i<runs; i++) {
    // produce
    std::this_thread::sleep_for(std::chrono::milliseconds(time_msec));
    // access the queue and add an item to it
    std::lock_guard<std::mutex> guard(shared_queue_mutex);
    //shared_queue_mutex.lock();

    shared_queue.push(my_id);
    std::cout << "PROD " << my_id << " added an item to the queue" << std::endl;
    //shared_queue_mutex.unlock();
  }
}

void consumer(int time_msec, int runs) {
  std::thread::id my_id = std::this_thread::get_id();
  for (int i=0; i<runs; i++) {
    std::thread::id task;
    bool task_taken = false;
    // access the queue and add an item to it
    while (!task_taken) {
      std::lock_guard<std::mutex> guard(shared_queue_mutex);
      //shared_queue_mutex.lock();
      if (shared_queue.size()>0) {
        task = shared_queue.front();
        shared_queue.pop();
        task_taken = true;
        std::cout << "CONS " << my_id << " removed item "<< task <<" from the queue." << std::endl;
      }
      //shared_queue_mutex.unlock();
    }
    // consume
    std::this_thread::sleep_for(std::chrono::milliseconds(time_msec));
  }
}


int main(int argc, char **argv) {

  if (argc!=3) {
    std::cout << "Usage: prod-cons <producers> <consumers>" << std::endl;
    return 1;
  }

  int n_prod = atoi(argv[1]);
  int n_cons = atoi(argv[2]);

  std::cout << "N. Producers: " << n_prod << std::endl;
  std::cout << "N. Consumers: " << n_cons << std::endl;

  std::vector<std::thread> producers(n_prod);
  std::vector<std::thread> consumers(n_cons);

  for (int i = 0; i < n_prod; i++)
    producers[i] = std::thread(producer, rand()%1000, 10);

  for (int i = 0; i < n_cons; i++)
    consumers[i] = std::thread(consumer, rand()%1000, 10);

  for (int i = 0; i < n_prod; i++)
    producers[i].join();

  for (int i = 0; i < n_cons; i++)
    consumers[i].join();

  return 0;
}


