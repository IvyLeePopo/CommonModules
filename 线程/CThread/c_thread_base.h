/*
 * @Description: C�߳�
 * @Author: Ivy
 * @Date: 2022-05-11 09:34:43
 * @LastEditTime: 2022-05-11 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */


#pragma once
#include <Windows.h>
class CThreadBase
{
public:
	CThreadBase(void);
	virtual ~CThreadBase(void);

public:
	// �����߳�
	void vStart();

	// �����߳�
	virtual void vStop();

	// �߳��Ƿ������
	virtual bool isCanRun();

	// ������д�˺������ڽ����߳��߼�
	virtual int vThreadWork();

	// �����������ٽ�������ͬ�ı��뻷���޸ĳ�������ͬ����������
	void _lock();
	void _unLock();
private:
protected:
	DWORD m_dThreadId;						// �߳�ID	
	bool m_bCanRun;							// �Ƿ��������
	int m_nThreadHandle;					// �߳̾��
#ifdef WINVER
	CRITICAL_SECTION m_criSection;		// �ٽ���������һ������Ҫ����ͬ������ȡ����
#endif
};
