/*
 * @Description: C���Ե�Socket Api�Ľ��ܣ��Լ�һЩ�������漰��api
 * @Author: Ivy
 * @Date: 2022-04-27 09:34:43
 * @LastEditTime: 2023-02-27 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */


#include <winsock2.h>
#include <ws2tcpip.h>
#include <IPHlpApi.h>

// 1��try_lock_for��C++11�е�һ��������������ָ��ʱ���ڳ��Ի�ȡһ����������
template< class Rep, class Period >
bool try_lock_for(const std::chrono::duration<Rep, Period>& timeout_duration);

// 2����һ��I/O��·���û��ƣ�����ͬʱ���Ӷ���ļ���������״̬
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

// 3����һ�����������׽���ѡ���ϵͳ����
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);

// 4���ڽ���socketͨѶʱ������ʹ��ioctlsocket���������úͻ�ȡsocket�����Ժ�״̬
int ioctlsocket(SOCKET s, long cmd, u_long* argp);


/**
��д����˵�ʱ�򣬿�����libevent����������ͨ��IO֪ͨ�ķ�ʽ����ÿһ���ͻ��˵���Ϣ���򵥵�һ̨�������������ɹ���ʮ����Ŀͻ�������
����һ�£����Ǹ�������д����ˣ���ο��ټ�ʱ�Ĺ�����ô��Ŀͻ��ˣ��ѵ�Ҫ��һ��whileѭ����߲��϶�ȡ��
IO���û��ƣ��������������������
������Ǵ�����ģ�������Ҫ������ĳһ���������ķ�������
����ĳһ���������ĺ��������������ã�����ǿ��Է�װ�ɹ��ú�������������helper���
��������ҵ�񡢿��ơ����ݡ����������з�������
���ܸ���Դ����㣬��ʱ������Լ��������˵ļܹ���ƣ���github��  srs
��Ҫ���ܿ��ö�����⣬���õ��Լ�����Ŀ���У�lamed���ʽ��Э�̵ȵ��µ�֪ʶ
����Ҳ��һ���������ƣ�gitee.com
*/