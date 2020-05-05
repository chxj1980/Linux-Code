#include <pthread.h>
#include <unistd.h>
#include <iostream>
using namespace std;

int number = 0;


class ReadWriteLock {
private:
  pthread_rwlock_t rwlock_;
public: 
  ReadWriteLock() {
    int res = pthread_rwlock_init(&rwlock_, NULL);
    if (res != 0) {
      perror("pthread_rwlock_init");
    }
  } 

  ~ReadWriteLock() {
    int res = pthread_rwlock_destroy(&rwlock_);
    if (res != 0) {
      perror("pthread_rwlock_destroy");
    }
  }

  bool readlock() {
    return pthread_rwlock_rdlock(&rwlock_) == 0; 
  }

  bool writelock() {
    return !pthread_rwlock_wrlock(&rwlock_);
    
  }

  bool readtrylock() {
    return !pthread_rwlock_tryrdlock(&rwlock_);
  }

  bool writetrylock() {
    return !pthread_rwlock_trywrlock(&rwlock_);
  }

  bool unlock() {
    return !pthread_rwlock_unlock(&rwlock_);
  }
};

ReadWriteLock lock;

void *writeTask(void* arg) {
  while (1) {
    lock.writelock();
    number ++;
    cout << "write: " << pthread_self() << ", number = " << number << endl;
    lock.unlock();
    usleep(500);
  }
  return NULL;
}

void *readTask(void* arg) {
  while (1) {
    lock.readlock();
    cout << "read: " << pthread_self() << ", number = " << number << endl;
    lock.unlock();
    usleep(500);
  }
  return NULL;
}


int main() {
  pthread_t threads[8];
  
  // 创建三个写线程
  for (int i=0; i<3; ++i) {
    pthread_create(&threads[i], NULL, writeTask, NULL);
  }
  for (int i=3; i<8; ++i) {
    pthread_create(&threads[i], NULL, readTask, NULL);
  }

  // 阻塞回收子线程的pcb资源
  for (int i=0; i<8; ++i) {
    pthread_join(threads[i], NULL);
  }
  return 0;
}