#include "CSocketApi.h"

/*****************************************************************
 函数名称：try_lock_for
 函数描述：是C++11中的一个函数，用于在指定时间内尝试获取一个互斥锁
 输入参数：timeout_duration是一个std::chrono::duration类型的参数，表示等待的时间;
 输出参数：
 返回说明：如果在等待时间内成功获取到锁，则返回true；否则返回false
 其它说明：使用try_lock_for函数可以避免在等待锁时出现死锁的情况。
	      例如，当一个线程在等待获取一个锁时，如果另一个线程已经持有了该锁并且正在执行，
		  那么第一个线程就会一直等待下去，直到第二个线程释放了锁。
		  但是，如果使用try_lock_for函数，在等待一定时间后如果还没有获取到锁，
		  第一个线程就可以放弃等待并执行其他操作，避免了死锁的情况。
 *****************************************************************/
template<class Rep, class Period>
inline bool try_lock_for(const std::chrono::duration<Rep, Period>& timeout_duration)
{
	return false;
}

/*****************************************************************
 函数名称：select
 函数描述：是一个在多路I/O中使用的系统调用函数，它用于检查一组文件描述符的状态变化它可以同时检查多个文件描述符，
	      当其中任意一个文件描述符发生读写等事件时，select()函数就会返回，并且告知哪些文件描述符发生了变化。
 输入参数：nfds是待检查的文件描述符的数量，它是待检查文件描述符的最大值+1;
          参数readfds、writefds和exceptfds是指向描述符集合的指针;
		  timeout用于设置select()函数的超时时间，如果设置为NULL，则select()函数将一直阻塞，直到有文件描述符发生变化为止。
 输出参数：
 返回说明：返回值为就绪的文件描述符个数;
	      如果返回值为0，则表示超时；
		  如果返回值为-1，则表示select()函数执行出错。
 其它说明：select()函数可以监视三个描述符集合：readfds（可读集合），writefds（可写集合）和exceptfds（异常集合）
          使用select()函数可以实现非阻塞I/O，因为它可以在多个文件描述符中选择一个可读或可写的描述符，
		  而不必阻塞等待某个特定的文件描述符可读或可写。这样可以提高系统的并发性和响应性。
 *****************************************************************/
int select(int nfds, fd_set * readfds, fd_set * writefds, fd_set * exceptfds, timeval * timeout)
{
	int iEvtCnt;
	fd_set stWriteSet, stExceptSet;
	struct timeval stTV;
	stTV.tv_sec = timeout / 1000;
	stTV.tv_usec = (timeout % 1000) * 1000;
	FD_ZERO(&stWriteSet);
	FD_ZERO(&stExceptSet);// 将fd_set清零，即将数组中所有元素置为0
	FD_SET(m_Socket, &stWriteSet);// 将指定的文件描述符fd添加到fd_set中，即将对应的数组元素置为1。
	FD_SET(m_Socket, &stExceptSet);
	iEvtCnt = select((int)m_Socket + 1, NULL, &stWriteSet, &stExceptSet, &stTV);  // Wait on read or error

	if ((iEvtCnt <= 0) || (FD_ISSET(m_Socket, &stExceptSet)))//FD_ISSET：判断文件描述符fd对应的位是否为1
	{
		Disconnect();
		return false;
	}
}


/*****************************************************************
 函数名称：setsockopt
 函数描述：
 输入参数：sockfd：要设置选项的套接字文件描述符。
		  level：选项所在协议的层次。常用的值有 SOL_SOCKET（表示套接字选项）、IPPROTO_TCP（表示 TCP 协议选项）、IPPROTO_IP（表示 IP 协议选项）等。
		  optname：选项名称。常用的选项名称包括 SO_REUSEADDR（重用地址）、SO_KEEPALIVE（保持连接）、TCP_NODELAY（禁用 Nagle 算法）等。
		  optval：指向选项值的指针。
		  optlen：选项值的长度。
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int setsockopt(int sockfd, int level, int optname, const void * optval, socklen_t optlen)
{
	int optval = 1;
	int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	if (ret < 0) 
	{
		perror("setsockopt failed");
		exit(EXIT_FAILURE);
	}
}
