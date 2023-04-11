/*
 * pi-pthreads.cc
 *
 *  Created on: Oct 18, 2016
 *      Author: claudio
 */

// g++-6 -std=c++11 -O3 -lpthread pi-pthreads.cc -o pi-pthreads

#include <pthread.h>
#include <iostream>
#include <chrono>
#include <vector>

#define STEPS 1000000000

struct params { int id; int n; int skip; double result; };


void* PIworker(void *arg) {
  params* data = (params*) arg;
  double d = 1.0/(double)data->n;
  double s = 0.0, x;
  for (int i=data->id+1; i<=data->n; i+=data->skip) {
    x = (i-0.5)*d;
    s += 4.0/(1.0+x*x);
  }
  data->result = d*s;

  pthread_exit(NULL);
}

int main(int argc, char **argv) {
  const int NUM_THREADS = atoi(argv[1]);

  std::vector<pthread_t> tid(NUM_THREADS);
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  params shared[NUM_THREADS];

  std::cout << "Starting..." << std::endl;
  // initializzo chrono
  auto begin = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < NUM_THREADS; i++) {
    shared[i].id  = i;
    shared[i].n    = STEPS;
    shared[i].skip = NUM_THREADS;

    pthread_create(&tid[i], &attr, &PIworker, &shared[i]);
  }

  std::cout << "waiting..." << std::endl;
  for (int i = 0; i < NUM_THREADS; i++)
    pthread_join(tid[i], NULL);
  double pi = 0;
  for (int i=0; i<NUM_THREADS; i++)
    pi += shared[i].result;

  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds> (end - begin);

  std::cout << "     PI: " << pi << std::endl;
  std::cout << "Elapsed: " << elapsed.count() << " ms." << std::endl;

  pthread_exit(NULL);

  return 0;
}


