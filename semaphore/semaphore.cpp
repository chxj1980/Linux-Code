#include <semaphore.h>
#include <pthread.h>
#include <iostream>
#include <unistd.h>
using namespace std;


sem_t consumer_sem, producer_sem;

struct Node {
  int val;
  Node *next;

  Node(int val, Node *next) {
    this->val = val;
    this->next = next;
  }
}; 

Node *head = nullptr;

void *consumer(void* arg) {
  while (1) {
    sem_wait(&consumer_sem); // 阻塞
    Node *p = head;
    head = head->next;
    cout << "consumer: " << pthread_self() << ":" << p->val << endl;
    delete p;
    p = nullptr;
    sem_post(&producer_sem);
    sleep(rand() % 3);
  }
  return nullptr;
}

void *producer(void *arg) {
  while (1) {
    sem_wait(&producer_sem);
    Node *p = new Node(rand()%100, head);
    head = p;
    cout << "producer: " << pthread_self() << ":" << p->val << endl;
    sem_post(&consumer_sem);
    sleep(rand() % 3);
  }

  return nullptr;
}

/*
生产者对应一个信号量
消费者对应一个信号量
*/


int main() {
  pthread_t t1, t2;
  pthread_create(&t1, nullptr, consumer, nullptr);
  pthread_create(&t2, nullptr, producer, nullptr);
  sem_init(&consumer_sem, 0, 0);
  sem_init(&producer_sem, 0, 4);

  pthread_join(t1, nullptr);
  pthread_join(t2, nullptr);
  sem_destroy(&consumer_sem);
  sem_destroy(&producer_sem);

  while (head) {
    Node *cur = head->next;
    delete head;
    head = nullptr;
    head = cur;
  }

  return 0;
}
