/*
 * @Description: 的用法
 * @Author: Ivy
 * @Date: 2022-05-10 09:34:43
 * @LastEditTime: 2022-05-10 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once

#include <QMutex>
#include <QObject>
#include <QSet>
#include <QThread>
#include <QVariantMap>

#define bindThreadFunc(a) std::bind(&a, this)
#define ThreadFunc std::function<void()>

class Thread;
class ThreadManager : public QObject
{
	Q_OBJECT
public:
	ThreadManager(QObject *parent);
	~ThreadManager();
public:
	void init();
	bool starThread(Thread *thread);
	int getThreadCount();
	void quitAllThread();
protected slots:
	void onThreadFinish();
signals:
	void allThreadQuit();
private:
	QSet<Thread *>m_setThreads;
	QMutex          m_theadsLock;
	bool            m_quit;
};

class Thread : public QThread
{
	Q_OBJECT
public:
	explicit Thread(const QString &threadName, QObject *parent = nullptr);
	virtual ~Thread();
	friend class ThreadManager;

private:
	Thread();

private:
	virtual void run() override;
	// 继承需要调用的方法
	virtual void doRun();

public:
	// 不需要放到线程管理类中时调用的
	void startAutoDestroy();
	void setThreadName(const QString &threadName);
	void setThreadFunc(ThreadFunc threadFunc);
	QString getThreadName();
	// 判断是否要退出线程，给调用者判断
	bool needQuit();
	void quitThread();

signals:
	void _threadFinished(QVariantMap result);

private:
	void quit();
	void start(Priority priority = InheritPriority);
	void terminate();

private:
	ThreadFunc  m_threadFunc;
	QString     m_threadName;
	bool        m_quit;
};
