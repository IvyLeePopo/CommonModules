/*
 * @Description: 单例类工厂
 * @Author: Ivy
 * @Date: 2022-04-09 09:34:43
 * @LastEditTime: 2023-02-24 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */
#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <QMutex>

using namespace std;
template <typename T>
class Singleton
{
public:
    static T* getInstance()
    {
        if (m_pInstance == NULL)
        {
            // 二次保证线程安全
            m_pMutexCreate.lock();
            if (m_pInstance == NULL)
            {
                m_pInstance = new T();
            }
            m_pMutexCreate.unlock();
        }
        return m_pInstance;
    }

    // 尽量多的将单例共有的方法放到基类，子类继承即可
    virtual void startWork() {}
protected:
    Singleton() {}
    virtual ~Singleton()
    {
        if (m_pInstance != NULL)
        {
            delete [] m_pInstance;
            m_pInstance = NULL;
        }
    }

    static T* m_pInstance;
    static QMutex m_pMutexCreate;
    QMutex m_mutex;
private:
    Singleton(const Singleton&);
    Singleton& operator =(const Singleton&);

public:
    void lock() { m_mutex.lock(); }
    void unLock() { m_mutex.unlock(); }
};

template <typename T>
QMutex Singleton<T>::m_pMutexCreate;

template <typename T>
T* Singleton<T>::m_pInstance = NULL;
