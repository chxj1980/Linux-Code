### 进程回收

#### 1.wait-阻塞函数
##### (1)阻塞并且等待子进程退出
##### (2)回收子进程残留资源
##### (3)获取子进程结束状态(退出原因)
```
pid_t wait(int *status);
返回值:
  -1 : 回收失败，已经没有子进程了
  >0 : 回收的子进程对应的pid
参数:
  判断子进程是如何死
    正常退出
    被信号杀死
调用一次只能回收一个子进程

参数：子进程的退出状态 -- 是一个传出参数
WIFEXISTED(status) 为非零 -> 进程正常结束
  WEXITSTATUS(status) 获取退出状态(exit/return)
WIFSIGNALED(status) 为非零-> 进程异常终止
  WTERMSIG(status) 获取进程终止的那个信号的编号
```

#### 1.waitpid
```
pid_t waitpid(pid_t pid, int *status, int option);
参数：
  (1)pid   有选择的回收
    pid ==-1  回收所有的子进程 
      回收所有的子进程  循环回收
      while ((pid = waitpid(-1, &status, 0)) != -1)
    pid > 0   等待其进程ID与pid相等的子进程
    pid == 0  回收当前进程组的所有的子进程
    pid < 0   子进程的pid取反
  (2)status 子进程的退出状态
  (3)option 设置为WNOHANG,函数非阻塞;设置为0,函数阻塞
返回值：
  >0 返回清理掉的子进程pid
  -1 没有子进程
  如果为非阻塞
    ==0 子进程正在运行   
```