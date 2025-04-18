# RPC-
异步rpc框架开发
### 1:日志模块开发
  
  日志模块
、、、、
  1.日志等级
  2.打印到文件，支持日期令名，以及日志的滚动
  3.c格式化风控
  4.线程安全
、、、、

Loglevel
、、、、
    Dubug
    Info
    Error
、、、、


LogEvent
、、、、
文件名，行号
进程号，线程号
日期以及事件
自定义消息
、、、、

日志格式
、、、、
[%Event_level]][%y-%m-%d %H:%M:%s.%ms]\t[pid:thread_id]\t[file_name:line][%msg]

、、、、

### 2.1:Eventloop开发





### 2.2timer 开发

定时器：是timerEvent的集合
timer继承fdevent


如何判断定时如何是否需要执行
如何让epoll监听定时器
1：指定时间点 arrive_time
2：interval，ms
3：is_repeated
4:is_cancled
5:task

需要继承为fd_fd_event

### 2.3 IO线程

