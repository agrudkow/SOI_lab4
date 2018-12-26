#ifndef MONITOR_H
#define MONITOR_H

#include <iostream>
#include <thread>             // std::thread
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable
#include <time.h>

using namespace std;

struct Applicant {
  int id, priority;
};

class Monitor{
	condition_variable full, empty;
	mutex mtx;
	struct Applicant *buffor;
  // back - points on a place ready to be filled, except the feckt when queue is empty
  // front - points on a first element in a queue
	int buff_size, front, back, actual_size;

public:
	Monitor(int size);
	~Monitor();
  void push(int id, int priority);
	void push_priority(int id, int priority);
	struct Applicant pop();
	int get_size();
  void print();

private:
  int mod_m(int a, int b);
};


#endif
