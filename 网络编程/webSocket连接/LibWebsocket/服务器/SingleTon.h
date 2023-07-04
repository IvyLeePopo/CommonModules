/*
	单例类
*/
#pragma once
#include <map>
#include <string>
#include <vector>
#include <afxmt.h>
using namespace std;

template <typename T>
class SingleTon
{
public:
	static T* getInstance()
	{
		if(NULL == m_pInstance)
		{
			//m_singleCS.Lock();
			if(NULL == m_pInstance)
			{
				m_pInstance = new T();
			}
			//m_singleCS.Unlock();
		}
		return m_pInstance;
	}

protected:
	SingleTon(void){}
	virtual ~SingleTon(void){
		if(NULL != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}

	static T* m_pInstance;
	
private:
	SingleTon(const SingleTon&);
	SingleTon& operator = (const SingleTon&);
};

template <typename T>
T* SingleTon<T>::m_pInstance = NULL;