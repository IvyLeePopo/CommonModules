/*
 * @Description: C语言的Socket Api的介绍，以及一些网络编程涉及的api
 * @Author: Ivy
 * @Date: 2022-04-27 09:34:43
 * @LastEditTime: 2023-02-27 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */


#include <winsock2.h>
#include <ws2tcpip.h>
#include <IPHlpApi.h>

// 1、try_lock_for是C++11中的一个函数，用于在指定时间内尝试获取一个互斥锁。
template< class Rep, class Period >
bool try_lock_for(const std::chrono::duration<Rep, Period>& timeout_duration);

// 2、是一种I/O多路复用机制，可以同时监视多个文件描述符的状态
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

// 3、是一个用于设置套接字选项的系统调用
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);

// 4、在进行socket通讯时，可以使用ioctlsocket函数来设置和获取socket的属性和状态
int ioctlsocket(SOCKET s, long cmd, u_long* argp);


/**
在写服务端的时候，可以用libevent来做监听，通过IO通知的方式接收每一个客户端的消息，简单的一台服务器轻轻松松管理十几万的客户端链接
你想一下，让那个是让你写服务端，如何快速即时的管理那么多的客户端，难道要在一个while循环里边不断读取吗？
IO复用机制，大大提升了网络吞吐量
如果不是带界面的，尽量不要用依赖某一个编译器的方法、库
依赖某一个编译器的函数方法，就算用，最好是可以封装成公用函数，放在类似helper里边
尽量进行业务、控制、数据、编译器特有方法分离
介绍个开源库给你，有时间可以自己看看别人的架构设计，上github搜  srs
你要是能看得懂，理解，并用到自己的项目当中，lamed表达式、协程等等新的知识
国内也有一个，叫码云，gitee.com
*/