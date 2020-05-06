#include <pthread.h>
#include <iostream>
#include <unistd.h>
using namespace std;

struct Node {
  int val;
  Node *next;

  Node (int val, Node *next) {
    this->val = val;
    this->next = next;
  }
};

Node *head = nullptr;
pthread_mutex_t mtx;
pthread_cond_t cond;

void* consumer(void *arg) {
  while (1) {
    pthread_mutex_lock(&mtx);
    if (!head) {
      // continue; 忙轮询
      // 1.线程阻塞
      // 2.该函数会对互斥锁解锁
      pthread_cond_wait(&cond, &mtx);
      // 3.解除阻塞之后，对互斥锁进行加锁操作
    }
    Node *res = head;
    head = res->next;
    cout << "consumer: "<< pthread_self() << ":" << res->val << endl;
    pthread_mutex_unlock(&mtx);
    delete res;
    res = nullptr;
  }
  return nullptr;
}

void* producer(void *arg) {
  while (1) {
    pthread_mutex_lock(&mtx);
    Node *p = new Node(rand()%100, head);
    head = p;
    cout << "producer: " << pthread_self() << ":" << p->val << endl;
    pthread_mutex_unlock(&mtx);

    // 通知阻塞的消费者线程
    pthread_cond_signal(&cond);
    sleep(rand() % 3);
  }
  return nullptr;
}

int main() {
  pthread_t t1,t2;
  pthread_create(&t1, nullptr, consumer, nullptr);
  pthread_create(&t2, nullptr, producer, nullptr);
  pthread_cond_init(&cond, NULL);
  pthread_mutex_init(&mtx, NULL);

  pthread_join(t1, nullptr);
  pthread_join(t2, nullptr);

  while (head) {
    Node *cur = head->next;
    delete head;
    head = nullptr;
    head = cur;
  }


  pthread_cond_destroy(&cond);
  pthread_mutex_destroy(&mtx);
  return 0;
}