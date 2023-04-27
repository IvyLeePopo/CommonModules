/*
 * @Description: C���Ե�Socket Api�Ľ��ܣ��Լ�һЩ�������漰��api
 * @Author: Ivy
 * @Date: 2022-04-27 09:34:43
 * @LastEditTime: 2023-02-27 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

//try_lock_for��C++11�е�һ��������������ָ��ʱ���ڳ��Ի�ȡһ����������
template< class Rep, class Period >
bool try_lock_for(const std::chrono::duration<Rep, Period>& timeout_duration);

//��һ��I/O��·���û��ƣ�����ͬʱ���Ӷ���ļ���������״̬
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
