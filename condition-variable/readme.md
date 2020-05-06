### 读写锁，互斥锁的缺点
#### 阻塞线程

## 1.条件变量是锁吗？
不是锁，但是条件变量能够阻塞线程
使用条件变量+互斥锁
  互斥锁：保护一块共享数据
  条件变量：引起阻塞
  生产者和消费者模型
## 2.条件变量的两个动作？
  条件不满足，阻塞线程
  当条件满足时，通知阻塞的线程开始工作    信号 ——》通知一个  广播-》通知所有
## 3.条件变量的类型：pthread_cond_t;

## 4.主要函数
1.初始化条件变量
pthread_cond_init(pthread_cond_t *restrict cond, const pthread_condattr_t *restrict attr);

2.销毁一个条件变量
int pthread_cond_destroy(pthread_cond_t *cond);

3.限时等待
int pthread_cond_timedwait(pthread_cond_t *restrict cond,
           pthread_mutex_t *restrict mutex,
           const struct timespec *restrict abstime);

4.阻塞等待一个条件变量
int pthread_cond_wait(pthread_cond_t *restrict cond,
           pthread_mutex_t *restrict mutex);
  阻塞线程
  将已经上锁的Mutex解锁
  解除阻塞后，对互斥锁进行加锁

5.唤醒至少一个阻塞在条件变量上的线程
int pthread_cond_signal(pthread_cond_t *cond);

6.唤醒全部阻塞在条件变量上的线程
int pthread_cond_broadcast(pthread_cond_t *cond);

## 5.练习
使用条件变量实现生产者-消费者模型
