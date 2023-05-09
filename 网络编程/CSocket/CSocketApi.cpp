#include "CSocketApi.h"

/*****************************************************************
 �������ƣ�try_lock_for
 ������������C++11�е�һ��������������ָ��ʱ���ڳ��Ի�ȡһ��������
 ���������timeout_duration��һ��std::chrono::duration���͵Ĳ�������ʾ�ȴ���ʱ��;
 ���������
 ����˵��������ڵȴ�ʱ���ڳɹ���ȡ�������򷵻�true�����򷵻�false
 ����˵����ʹ��try_lock_for�������Ա����ڵȴ���ʱ���������������
	      ���磬��һ���߳��ڵȴ���ȡһ����ʱ�������һ���߳��Ѿ������˸�����������ִ�У�
		  ��ô��һ���߳̾ͻ�һֱ�ȴ���ȥ��ֱ���ڶ����߳��ͷ�������
		  ���ǣ����ʹ��try_lock_for�������ڵȴ�һ��ʱ��������û�л�ȡ������
		  ��һ���߳̾Ϳ��Է����ȴ���ִ�����������������������������
 *****************************************************************/
template<class Rep, class Period>
inline bool try_lock_for(const std::chrono::duration<Rep, Period>& timeout_duration)
{
	return false;
}

/*****************************************************************
 �������ƣ�select
 ������������һ���ڶ�·I/O��ʹ�õ�ϵͳ���ú����������ڼ��һ���ļ���������״̬�仯������ͬʱ������ļ���������
	      ����������һ���ļ�������������д���¼�ʱ��select()�����ͻ᷵�أ����Ҹ�֪��Щ�ļ������������˱仯��
 ���������nfds�Ǵ������ļ������������������Ǵ�����ļ������������ֵ+1;
          readfds��writefds��exceptfds��ָ�����������ϵ�ָ��;
		  timeout��������select()�����ĳ�ʱʱ�䣬�������ΪNULL����select()������һֱ������ֱ�����ļ������������仯Ϊֹ��
					��select()�����У����Խ�����ṹ����Ϊ�������ݸ���������ָ���ȴ����ʱ�䡣
					�����ָ����ʱ����û���κ�I/O�¼���������select()�����᷵��0��������ִ����򷵻�-1��
 ���������
 ����˵��������ֵΪ�������ļ�����������;
	      �������ֵΪ0�����ʾ��ʱ��
		  �������ֵΪ-1�����ʾselect()����ִ�г���
 ����˵����select()�������Լ����������������ϣ�readfds���ɶ����ϣ���writefds����д���ϣ���exceptfds���쳣���ϣ�
          ʹ��select()��������ʵ�ַ�����I/O����Ϊ�������ڶ���ļ���������ѡ��һ���ɶ����д����������
		  �����������ȴ�ĳ���ض����ļ��������ɶ����д�������������ϵͳ�Ĳ����Ժ���Ӧ�ԡ�
 *****************************************************************/


/**
Ϊɶ����select��ȴ� ֱ��recvЧ��Ҫ�첻��
-----һ������£�ʹ��select��ȴ�ֱ��recvЧ��Ҫ�죬��Ϊselect����ͬʱ��������ļ���������
		�������ݿɶ�ʱ���Ż���ж�ȡ���������Ա��������ȴ�������ֱ��recv��ֻ����Ե����ļ����������ж�ȡ��
		��Ҫ������ѯ����ȷ���Ƿ������ݿɶ���Ч�ʽϵ͡�����һЩ�ض�����£�ʹ�ô�ֱ��recv���ܻ����Ч��
		����ֻ��Ҫ����һ���ļ�������������������С������¡�
--------�������̫ǳ�ˣ�Ӧ��˵���������߳��ϲ�ĵ��ã���io����ֱ�Ӿ���ϵͳ�ײ�ִ�еģ�Ч���ǿ϶������

*/
int select(int nfds, fd_set * readfds, fd_set * writefds, fd_set * exceptfds, timeval * timeout)
{
	//ʹ�÷�����
	int iEvtCnt;
	fd_set stWriteSet, stExceptSet;
	struct timeval stTV;
	stTV.tv_sec = timeout / 1000;
	stTV.tv_usec = (timeout % 1000) * 1000;
	FD_ZERO(&stWriteSet);
	FD_ZERO(&stExceptSet);// ��fd_set���㣬��������������Ԫ����Ϊ0
	FD_SET(m_Socket, &stWriteSet);// ��ָ�����ļ�������fd��ӵ�fd_set�У�������Ӧ������Ԫ����Ϊ1��
	FD_SET(m_Socket, &stExceptSet);
	iEvtCnt = select((int)m_Socket + 1, NULL, &stWriteSet, &stExceptSet, &stTV);  // Wait on read or error

	/**
	����һ�����ļ���readSet������գ�Ȼ��readSet��ӵ�m_nSocketFd��
	ͨ���������select��ȥ����readSet��Ҫ��readSet�б仯�ˣ���ȥ��readSet�����������
	Ҫ�ǳ������tvʱ�䣬��û��I/O�¼��������ͻ᷵��0
	*/

	if ((iEvtCnt <= 0) || (FD_ISSET(m_Socket, &stExceptSet)))//FD_ISSET���ж��ļ�������fd��Ӧ��λ�Ƿ�Ϊ1
	{
		Disconnect();
		return false;
	}
}


/*****************************************************************
 �������ƣ�setsockopt
 ����������
 ���������sockfd��Ҫ����ѡ����׽����ļ���������
		  level��ѡ������Э��Ĳ�Ρ����õ�ֵ�� SOL_SOCKET����ʾ�׽���ѡ���IPPROTO_TCP����ʾ TCP Э��ѡ���IPPROTO_IP����ʾ IP Э��ѡ��ȡ�
		  optname��ѡ�����ơ����õ�ѡ�����ư��� SO_REUSEADDR�����õ�ַ����SO_KEEPALIVE���������ӣ���TCP_NODELAY������ Nagle �㷨���ȡ�
		  optval��ָ��ѡ��ֵ��ָ�롣
		  optlen��ѡ��ֵ�ĳ��ȡ�
 ���������
 ����˵����
 ����˵����
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

/*****************************************************************
 �������ƣ�ioctlsocket
 �������������úͻ�ȡsocket�����Ժ�״̬
 ���������sΪҪ�������Ե�socket��
		  cmdΪҪ���õ��������ͣ�
							1�����õ����������У�
								FIONBIO������socketΪ������ģʽ
								FIONREAD����ȡ���ջ������е����ݴ�С
								SIO_KEEPALIVE_VALS������TCP keep-alive����
							2�����õ�״̬�����У�
								SIO_GET_EXTENSION_FUNCTION_POINTER����ȡ��չ����ָ��
								SIO_GET_INTERFACE_LIST����ȡ����ӿ��б�
								SIO_GET_INTERFACE_LIST_EX����ȡ��չ����ӿ��б�
		  argpΪҪ���õ�����ֵ��
 ���������
 ����˵����
 ����˵����
 *****************************************************************/
int ioctlsocket(SOCKET s, long cmd, u_long * argp)
{
	// 1������socket�����ԣ�����socketΪ������ģʽ
	u_long nonBlocking = 1;
	if (ioctlsocket(sock, FIONBIO, &nonBlocking) == SOCKET_ERROR) 
	{
		// ����ʧ��
	}

	//2����ȡsocket��״̬����ȡ����ӿ��б�
	INTERFACE_INFO interfaceList[10];
	unsigned long interfaceListSize = sizeof(interfaceList);
	if (ioctlsocket(sock, SIO_GET_INTERFACE_LIST, &interfaceListSize) == SOCKET_ERROR) 
	{
		// ��ȡʧ��
	}
	else 
	{
		// ��ȡ�ɹ�������interfaceList
	}
}
