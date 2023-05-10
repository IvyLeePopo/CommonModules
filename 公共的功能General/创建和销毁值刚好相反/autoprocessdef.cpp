
#include "pch.h"
#include "autoprocessdef.h"
#include <QObject>
//////////////////////////////////////////////////////////////////////////
// AutoBlockSignal
AutoBlockSignal::AutoBlockSignal(QObject *object)
	: m_object(object)
{
	m_object->blockSignals(true);
}
AutoBlockSignal::~AutoBlockSignal()
{
	m_object->blockSignals(false);
}
//////////////////////////////////////////////////////////////////////////
// ReleaseBlockAndAutoRestore
ReleaseBlockAndAutoRestore::ReleaseBlockAndAutoRestore(QObject *object)
	: m_object(object)
{
	if (m_object)
	{
		m_blockSig = m_object->signalsBlocked();
		if (m_blockSig)
			m_object->blockSignals(false);
	}
}
ReleaseBlockAndAutoRestore::~ReleaseBlockAndAutoRestore()
{
	if (m_object && m_blockSig)
	{
		m_object->blockSignals(m_blockSig);
	}
}
/////////////////////////////////////////////////////////////////////////
//CAutoHandle
#ifdef Q_OS_WIN
CAutoHandle::CAutoHandle(HANDLE handle)
	: m_handle(handle)
{
}
CAutoHandle::~CAutoHandle()
{
	if (m_handle)
	{
		::CloseHandle(m_handle);
		m_handle = nullptr;
	}
}
CAutoHandle &CAutoHandle::operator=(const CAutoHandle &other)
{
	m_handle = other.m_handle;
	return *this;
}
#endif
//////////////////////////////////////////////////////////////////////////
// AutoForbidWebFocus
bool AutoForbidWebFocus::m_forceFocus = true;
AutoForbidWebFocus::AutoForbidWebFocus()
{
	m_forceFocus = false;
}
AutoForbidWebFocus::~AutoForbidWebFocus()
{
	m_forceFocus = true;
}
bool AutoForbidWebFocus::isFocus()
{
	return m_forceFocus;
}
void AutoForbidWebFocus::setFocus(bool value)
{
	m_forceFocus = value;
}
