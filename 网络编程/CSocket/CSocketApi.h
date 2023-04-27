/*
 * @Description: C语言的Socket Api的介绍，以及一些网络编程涉及的api
 * @Author: Ivy
 * @Date: 2022-04-27 09:34:43
 * @LastEditTime: 2023-02-27 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

//try_lock_for是C++11中的一个函数，用于在指定时间内尝试获取一个互斥锁。
template< class Rep, class Period >
bool try_lock_for(const std::chrono::duration<Rep, Period>& timeout_duration);

//是一种I/O多路复用机制，可以同时监视多个文件描述符的状态
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
