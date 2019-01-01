#ifndef MONITOR_H
#define MONITOR_H

#include <iostream>
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable


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
	int buff_size, front, back, actual_size, priority_num;

public:
	Monitor(int size);
	~Monitor();
  void push(int id, int priority);            //Pushing applicant in to the end of the queue without priority
	void push_priority(int id, int priority);   //Pushing applicant in to the end of the queue with priority
	struct Applicant pop();                     //Poping applicant from the start of the queue
	int get_size();                             //Get number of applicants in the queue
  void print();                               //Print queue
  int get_priority_num();                     //Get number of priority applicants in the queue

private:
  int mod_m(int a, int b);                    //Custom modulo function
};


#endif
