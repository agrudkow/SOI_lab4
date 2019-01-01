#include <stdio.h>
#include "monitor.h"

#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define RESET "\x1B[0m"

Monitor::Monitor(int size)
	: buffor(new struct Applicant[size]), buff_size(size), front(0), back(0), priority_num(0), actual_size(0)
{}

Monitor::~Monitor(){
	delete[] buffor;
}

int Monitor::mod_m (int a, int b){
  int result = a % b;
  if (result >= 0) {
    return result;
  } else {
    return (b + result);
  }
}

//Print selected queue
void Monitor::print() {
  if (this->actual_size == 0) {
    for (size_t i = 0; i < 10; i++) printf("***");
    return;
  }
  for (int i = 0; i < (this->buff_size - this->actual_size) ; ++i){
    printf("***");
  }
  int j = this->back;
  j = mod_m(j - 1, this->buff_size);
  for (int k = 0; k < (this->actual_size); k++) {
    if (this->buffor[j].priority == 1)
      printf(RED "%3d" RESET, this->buffor[j].id);
    else
      printf(GREEN "%3d" RESET, this->buffor[j].id);
    j = mod_m(j - 1, this->buff_size);
  }
}

//Pushing applicant in to the end of the queue without priority
void Monitor::push(int id, int priority){
  unique_lock<mutex> lck(mtx);

  if (this->actual_size == this->buff_size)
    full.wait(lck);

  this->buffor[this->back].id = id;
  this->buffor[this->back].priority = priority;
  this->actual_size += 1;
  this->back = (this->back + 1) % this->buff_size;
	if (priority == 1)
		priority_num++;

  if (this->actual_size == 1)
    empty.notify_one();
}
//Pushing applicant in to the end of the queue with priority
void Monitor::push_priority(int id, int priority){
	unique_lock<mutex> lck(mtx);
  if (this->actual_size == 0) {
		lck.unlock();
    this->push(id, priority);
    return;
  }

  int j;

  if (this->actual_size == this->buff_size)
    full.wait(lck);

  j = mod_m(this->back - 1, this->buff_size);

	if (priority == 1)
		priority_num++;

  for (int i = 0; i < this->actual_size; i++) {
    if (this->buffor[j].priority >= priority ) {
      this->buffor[((j + 1) % this->buff_size)].id = id;
      this->buffor[((j + 1) % this->buff_size)].priority = priority;
      this->actual_size += 1;
      this->back = (this->back + 1) % this->buff_size;
      return;
    }
    this->buffor[((j + 1) % this->buff_size)].id = this->buffor[j].id;
    this->buffor[((j + 1) % this->buff_size)].priority = this->buffor[j].priority;
    j = mod_m(j - 1, this->buff_size);
  }
  this->buffor[((j + 1) % this->buff_size)].id = id;
  this->buffor[((j + 1) % this->buff_size)].priority = priority;
  this->actual_size += 1;
  this->back = (this->back + 1) % this->buff_size;
  return;
}
//Poping applicant from the start of the queue
struct Applicant Monitor::pop(){
  struct Applicant first;
  unique_lock<mutex> lck(mtx);

  if (this->actual_size == 0)
    empty.wait(lck);

  this->actual_size -= 1;
  first = this->buffor[this->front];
  this->front = ((this->front + 1) % this->buff_size);
	if (first.priority == 1)
		priority_num--;

  if (this->actual_size == (this->buff_size - 1))
    full.notify_one();
  return first;
}
//Get number of priority applicants in the queue
int Monitor::get_priority_num(){
	return priority_num;
}
//Get number of applicants in the queue
int Monitor::get_size(){
	return actual_size;
}
