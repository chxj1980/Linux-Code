### 1.读写锁是几把锁？
#### 一把锁  pthread_rwlock_t lock;
### 2.读写锁的类型：读锁 写锁
读锁 - 对内存做读操作
写锁 - 对内存做写操作


### 3.读写锁的特性
线程a加读锁成功，又来了三个线程，做读操作，可以加锁成功  -》 读时共享，并行处理
线程a加写锁成功，又来了三个线程，做读操作，三个线程阻塞  -》 写时独占
线程a加读锁成功，又来了b线程加写锁阻塞，又来了c线程加读锁阻塞 -》 
读写不能同时进行
写的优先级高
### 4.读写锁练习场景
1.线程a加写锁成功，线程b请求读锁
线程b阻塞
2.线程a持有读锁，线程b请求写锁
线程b阻塞
3.线程a用于读锁，线程b请求读锁
线程b成功 并行处理
4.线程a持有读锁，然后线程b请求写锁，然后线程c请求读锁
线程b阻塞
线程c阻塞 - 写的优先级高
a解锁 -》 b线程加写锁成功 -》b解锁 -》 c才能加读锁
b执行完c才能执行
5.线程a持有写锁，然后线程b请求读锁，然后携程c请求写锁
线程b阻塞 线程c阻塞
c先执行 然后b执行

### 5.读写锁适用场景？
互斥锁 - 不管读写都是串行的
读写锁 - 读并行，写串行
程序中的读操作的次数 》 写操作的次数
### 6.主要的操作函数？
1.初始化读写锁
int pthread_rwlock_init(pthread_rwlock_t *restrict rwlock,const pthread_rwlockattr_t *restrict attr);
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
2.销毁读写锁
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
3.加读锁
pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
阻塞：之前对这个把加了写锁的操作
4.尝试加读锁
pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
5.加写锁
pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
6.尝试加写锁
pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
7.解锁
pthread_rwlock_unlock((pthread_rwlock_t *rwlock);


### 三个线程不定时对同一个全局资源进行写操作，五个线程不定时对同一个全局资源做读操作
