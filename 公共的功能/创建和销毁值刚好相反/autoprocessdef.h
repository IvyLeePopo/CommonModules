/*
 * @Description: 的用法
 * @Author: Ivy
 * @Date: 2022-05-10 09:34:43
 * @LastEditTime: 2022-05-10 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once

#define Q_OS_WIN 1
class QObject;
#ifdef Q_OS_WIN
#include <Windows.h>
#endif
class AutoBlockSignal
{
public:
	AutoBlockSignal(QObject *object);
	~AutoBlockSignal();
public:
	QObject *m_object;
};
class ReleaseBlockAndAutoRestore
{
public:
	ReleaseBlockAndAutoRestore(QObject *object);
	~ReleaseBlockAndAutoRestore();
public:
	QObject *m_object;
	bool    m_blockSig;
};
////////////////////////////////////////////////////////////////////
#ifdef Q_OS_WIN
class CAutoHandle
{
public:
	CAutoHandle(HANDLE handle);
	~CAutoHandle();
public:
	CAutoHandle &operator=(const CAutoHandle &other);
private:
	HANDLE m_handle;
};
#endif
//////////////////////////////////////////////////////////////////////////
// AutoForbidWebFocus
class AutoForbidWebFocus
{
public:
	AutoForbidWebFocus();
	~AutoForbidWebFocus();
	static bool isFocus();
	static void setFocus(bool value);
private:
	static bool m_forceFocus;
};
