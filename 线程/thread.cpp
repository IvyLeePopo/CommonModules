
#include "pch.h"
#include "thread.h"
// ThreadManager //////////////////////////////////////////////////////////////////////////////////////////
ThreadManager::ThreadManager(QObject *parent)
	: QObject(parent)
	, m_quit(false)
{
}

ThreadManager::~ThreadManager()
{
}

void ThreadManager::init()
{
	QMutexLocker lock(&m_theadsLock);
	m_quit = false;
}

bool ThreadManager::starThread(Thread *thread)
{
	{
		QMutexLocker lock(&m_theadsLock);
		if (m_quit)
			return false;
		m_setThreads.insert(thread);
	}
	connect(thread, &Thread::finished, this, &ThreadManager::onThreadFinish);
	thread->start();
	return true;
}

int ThreadManager::getThreadCount()
{
	QMutexLocker lock(&m_theadsLock);
	return m_setThreads.size();
}

void ThreadManager::quitAllThread()
{
	{
		QMutexLocker lock(&m_theadsLock);
		m_quit = true;
	}
	for (auto iter = m_setThreads.begin(); iter != m_setThreads.end(); ++iter)
	{
		(*iter)->quitThread();
	}
}

void ThreadManager::onThreadFinish()
{
	Thread *thread = qobject_cast<Thread*>(sender());
	auto iter = m_setThreads.find(thread);
	if (iter != m_setThreads.end())
	{
		m_setThreads.erase(iter);
	}
	disconnect(thread);
	thread->deleteLater();
	if (m_setThreads.size() == 0)
	{
		emit allThreadQuit();
	}
}

// Thread //////////////////////////////////////////////////////////////////////////////////////////
Thread::Thread(const QString &threadName, QObject *parent)
	: QThread(parent)
	, m_threadName(threadName)
	, m_quit(false)
	, m_threadFunc(nullptr)
{

}

Thread::~Thread()
{

}

void Thread::run()
{
	doRun();
}

void Thread::doRun()
{
	if (m_threadFunc)
	{
		m_threadFunc();
	}
}

void Thread::startAutoDestroy()
{
	connect(this, &Thread::finished, this, &Thread::deleteLater);
	start();
}

void Thread::setThreadName(const QString &threadName)
{
	m_threadName = threadName;
}

void Thread::setThreadFunc(ThreadFunc threadFunc)
{
	m_threadFunc = threadFunc;
}

QString Thread::getThreadName()
{
	return m_threadName;
}

bool Thread::needQuit()
{
	return m_quit;
}

void Thread::quitThread()
{
	m_quit = true;
}

void Thread::quit()
{
	QThread::quit();
}

void Thread::start(Priority priority)
{
	QThread::start(priority);
}

void Thread::terminate()
{
	QThread::terminate();
}
