#ifndef MONITOR_H
#define MONITOR_H

#include <iostream>

struct Applicant {
  int id, priority;
};

class Monitor{
	conndition_variable full, empty;
	mutex mtx;
	struct Applicant *buffor;
  // back - points on a place ready to be filled, except the feckt when queue is empty
  // front - points on a first element in a queue
	int buff_size, front, head, actual_size;

public:
	Monitor(int size);
	~Monitor();
  void push();
	void push_priority(int id, innt priority);
	struct Applicant pop();
	int get_size();
  void print();

private:
  int mod_m(int a, int b);
};


#endif
