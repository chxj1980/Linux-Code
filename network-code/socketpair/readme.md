## socketpair

## unix域套接字传递文件描述符

- 父子进程  具有亲缘关系进程的通信， 与pipe不同的是 pipe是半双工的而socketpair是全双工的
```

socketpair
- 功能：创建一个全双工的流管道
- 函数原型：int socketpair(int domain, int type, int protocol, int sv[2]);
- 参数
  - doamin 协议家族
  - type 套接字类型
  - protocol 协议类型
  - sv 返回套接字对   传出参数
- 返回值
  成功返回0  失败返回-1
```

## sendmsg/recvmsg

```
ssize_t sendmsg(int sockfd, const struct msghdr* msg, int flags);
ssize_t recvmsg(int sockfd, struct msghdr* msg, int flags);
ssize_t writev(int fd, const struct iovec *iov, int iovcnt);
ssize_t readv(int fd, cvonst struct iovec *iov, int iovcnt);

struct msghdr {
  void         *msg_name;       
  socklen_t     msg_namelen;

  struct iovec *msg_iov;        
  size_t        msg_iovlen;      // 有几块缓冲区

  
  辅助数据 - 控制信息
  void         *msg_control;    
  size_t        msg_controllen; 
  int           msg_flags;   
};

// send(sockfd, buffer, len, flags);
// sendmsg();

struct iovec {
  void *iov_base;
  size_t iov_len;
};

struct cmsghdr {
  size_t cmsg_len;    // 整块数据的长度
  int    cmsg_level;   
  int    cmsg_type;   
  unsigned char cmsg_data[]; 
};