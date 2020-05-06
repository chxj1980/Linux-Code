### 1.头文件 semaphore.h
### 2.信号量类型  
  sem_t sem
  加强版的互斥锁

互斥锁->串行
信号量->并行


### 3.主要函数
1.初始化信号量
int sem_init(sem_t *sem, int pshared, unsigned int value)
pshared:
  0 - 线程同步
  1 - 进程同步
value - 最多有几个线程操作共享数据

2.销毁信号量
int sem_destroy(sem_t *sem);

3.加锁
int sem_wait(sem_t *sem);
p操作 sem == 0 阻塞   sem != 0 sem --

4.解锁
int sem_post(sem_t *sem);
sem ++;

5.尝试加锁
int sem_trywait(sem_t *sem);
sem == 0, 加锁失败，不阻塞，直接返回

6.限时尝试加锁
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);