#ifndef ASINGLETON_H
#define ASINGLETON_H

#include <QMutex>
#include <QMutexLocker>

//单例模板，子类继承时需将它设置为子类的友元类
template<typename T>
class ASingleton
{
public:
	static T& getInstance()
	{
		if(nullptr == m_pInstance)
		{
			QMutexLocker lock(&m_mutex);
			if(nullptr == m_pInstance)
			{
				T* pTmp = new T();
				m_pInstance = pTmp;
			}
		}
		return *m_pInstance;
	}

	static void release()
	{
		QMutexLocker lock(&m_mutex);
		if(nullptr != m_pInstance)
		{
			delete m_pInstance;
		}
		m_pInstance = nullptr;
	}

protected:
	ASingleton()
	{

	}

private:
	ASingleton(const ASingleton&)
	{

	}
	ASingleton& operator= (const ASingleton& rh)
	{
		return *this;
	}
private:
	static T* m_pInstance;
	static QMutex m_mutex;
};

template<typename T>
T* ASingleton<T>::m_pInstance = nullptr;

template<typename T>
QMutex ASingleton<T>::m_mutex;

#endif // ASINGLETON_H
