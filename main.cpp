#include <iostream>           // std::cout
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <memory>
#include "monitor.h"

#define NUM_OF_APPLICANTS 30
#define Q_SIZE    10
mutex print_mutex, mtxAB, mtxCD;
pthread_t U0_thread, U1_1_thread, U1_2_thread, U2_thread;
//Monitor *queue_A, *queue_B, *queue_C, *queue_D;
shared_ptr<Monitor> queue_A(new Monitor(Q_SIZE));
shared_ptr<Monitor> queue_B(new Monitor(Q_SIZE));
shared_ptr<Monitor> queue_C(new Monitor(Q_SIZE));
shared_ptr<Monitor> queue_D(new Monitor(Q_SIZE));
condition_variable queuesAB_full, queuesCD_empty;

using namespace std;

//Clean up all allocated memory and delete threads
void clean_up() {
  pthread_kill(U1_1_thread, 9);
  pthread_kill(U1_2_thread, 9);
/*
  delete queue_A;
  delete queue_B;
  delete queue_C;
  delete queue_D;
  */
}
//Prints all given queues aka simlation
void print_all() {
  unique_lock<mutex> lck(print_mutex);

  printf("\033c");
  printf("      ");
  queue_A->print();
  printf("[U1_1]");
  queue_C->print();
  printf("\n");
  printf("     /");
  for (size_t i = 0; i < 66; i++) printf(" ");
  printf("\\\n");
  printf("    /");
  for (size_t i = 0; i < 68; i++) printf(" ");
  printf("\\\n");
  printf("[U0]");
  for (size_t i = 0; i < 70; i++) printf(" ");
  printf("[U2]\n");
  printf("    \\");
  for (size_t i = 0; i < 68; i++) printf(" ");
  printf("/\n");
  printf("     \\");
  for (size_t i = 0; i < 66; i++) printf(" ");
  printf("/\n");
  printf("      ");
  queue_B->print();
  printf("[U1_2]");
  queue_D->print();
  printf("\n");
}

void* process_U0(void*) {
  int applicant_id = 1, applicant_priority, which_queue;
  srand(time(NULL));

  while (applicant_id <= NUM_OF_APPLICANTS) {
    if ((queue_A->get_size() + queue_B->get_size()) == 2 * Q_SIZE){
      unique_lock<mutex> lck(mtxAB);
      queuesAB_full.wait(lck);
    }
    which_queue = (int)(rand()) % 2; // 2 - number of queues betwen U0 and U1
    if (which_queue == 0 && queue_A->get_size() != Q_SIZE) {
      applicant_priority = rand() % 2;
      queue_A->push(applicant_id, applicant_priority);
      applicant_id++;
    } else if (which_queue == 1 && queue_B->get_size() != Q_SIZE) {
      applicant_priority = rand() % 2;
      queue_B->push(applicant_id, applicant_priority);
      applicant_id++;
    }
    print_all();
    //sleep(1);
  }
  pthread_exit(NULL);
}

void* process_U1_1(void*) {
  sleep(5);
  struct Applicant applicant;
  int last_size;

  for (;;) {
    last_size = queue_A->get_size() + queue_B->get_size();
    applicant = queue_A->pop();
    if (last_size == (2 * Q_SIZE - 1)){
      unique_lock<mutex> lck(mtxAB);
      queuesAB_full.notify_one();
    }

    print_all();
    sleep(1);

    queue_C->push_priority(applicant.id, applicant.priority);
    if ((queue_C->get_size() + queue_D->get_size()) == 1){
      unique_lock<mutex> lck(mtxCD);
      queuesCD_empty.notify_one();
    }

    print_all();
  }
}

void* process_U1_2(void*) {
  sleep(5);
  struct Applicant applicant;
  int last_size;

  for (;;) {
    last_size = queue_A->get_size() + queue_B->get_size();
    applicant = queue_B->pop();
    if (last_size == (2 * Q_SIZE - 1)){
      unique_lock<mutex> lck(mtxAB);
      queuesAB_full.notify_one();
    }

    print_all();
    sleep(1);

    queue_D->push_priority(applicant.id, applicant.priority);
    if ((queue_C->get_size() + queue_D->get_size()) == 1){
      unique_lock<mutex> lck(mtxCD);
      queuesCD_empty.notify_one();
    }

    print_all();
  }
}

void* process_U2(void*) {
  sleep(15);
  struct Applicant applicant;
  int priority_num, applicant_num = 0;

  for (;;) {
    if ((queue_C->get_size() + queue_D->get_size()) == 0){
      unique_lock<mutex> lck(mtxCD);
      queuesCD_empty.wait(lck);
    }
    //checks if there is more priority applicants in C queue. If that is true
    //it takes one applicant form C queue
    if (queue_C->get_priority_num() > queue_D->get_priority_num()
    && queue_C->get_priority_num() > 0) {
      applicant = queue_C->pop();
      applicant_num++;
    }
    //checks if there is more priority applicants in D queue. If that is true
    //it takes one applicant form D queue
    else if (queue_D->get_priority_num() > queue_C->get_priority_num()
    && queue_D->get_priority_num() > 0) {
      applicant = queue_D->pop();
      applicant_num++;
    }
    //now when nuber of priority applicants is equal in both queues algorithm
    //checks in which queues overall number of applicants is bigger
    else if (queue_C->get_size() > queue_D->get_size()) {
      applicant = queue_C->pop();
      applicant_num++;
    } else {
      applicant = queue_D->pop();
      applicant_num++;
    }
    print_all();
    sleep(1);
    if (applicant_num == NUM_OF_APPLICANTS){
      std::cout << "Koniec" << '\n';
      clean_up();
      pthread_exit(NULL);
    }
  }
}
//Initialize queues, threads
void init_all() {
  /*
  queue_A = new Monitor(Q_SIZE);
  queue_B = new Monitor(Q_SIZE);
  queue_C = new Monitor(Q_SIZE);
  queue_D = new Monitor(Q_SIZE);
  */

  pthread_create(&U0_thread, NULL, &process_U0, NULL);
  pthread_create(&U1_1_thread, NULL, &process_U1_1, NULL);
  pthread_create(&U1_2_thread, NULL, &process_U1_2, NULL);
  pthread_create(&U2_thread, NULL, &process_U2, NULL);
}

//Start threads
void simulate() {
  pthread_join(U0_thread, NULL);
  pthread_join(U1_1_thread, NULL);
  pthread_join(U1_2_thread, NULL);
  pthread_join(U2_thread, NULL);
}

int main(int argc, char const *argv[]) {
  init_all();
  simulate();

  return 0;
}
