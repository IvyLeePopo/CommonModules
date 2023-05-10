
//源文件
#include "pch.h"
#include "basepopupdialog.h"
#include "pdialog.h"
//////////////////////////////////////////////////////////////////////////////////////////////////
//BasePopupDialog
BasePopupDialog::BasePopupDialog(QWidget* parent)
	: QDialog(parent)
	, m_bFirstCheckMainMonitor(false)
{
	installEventFilter(this);
}

BasePopupDialog::~BasePopupDialog()
{
}

bool BasePopupDialog::wndIsVisible()
{
#ifdef Q_OS_WIN
	// 过滤这种窗口隐藏的情况
	if (!::IsWindowVisible((HWND)getAncestorWidgetId()))
		return true;
	m_listMonitorRect.clear();
	m_bFirstCheckMainMonitor = TRUE;
	::EnumDisplayMonitors(NULL, NULL, BasePopupDialog::MonitorEnumProc, (LPARAM)this);
	if (m_listMonitorRect.size() == 0)
		return true;
	bool bResult = false;
	RECT rcMain = {};
	::GetWindowRect((HWND)getAncestorWidgetId(), &rcMain);
	HRGN rgnMain = ::CreateRectRgn(rcMain.left, rcMain.top, rcMain.right, rcMain.bottom);
	RECT rcTaskBar = {};
	::GetWindowRect(::FindWindow(TEXT("Shell_TrayWNd"), NULL), &rcTaskBar);
	foreach(RECT rc, m_listMonitorRect)
	{
		// 过滤任务栏的情况
		RECT rcCurTaskBar = { rc.left, rcTaskBar.top, rc.right, rcTaskBar.bottom };
		HRGN rgnTaskBar = ::CreateRectRgn(rcCurTaskBar.left, rcCurTaskBar.top, rcCurTaskBar.right, rcCurTaskBar.bottom);
		HRGN rgnMonitor = ::CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
		::CombineRgn(rgnMonitor, rgnMonitor, rgnTaskBar, RGN_XOR);
		::CombineRgn(rgnMonitor, rgnMain, rgnMonitor, RGN_AND);
		RECT rcCombine = {};
		if (::GetRgnBox(rgnMonitor, &rcCombine) == SIMPLEREGION)
		{
			bResult = true;
		}
		::DeleteObject(rgnMonitor);
		::DeleteObject(rgnTaskBar);
		if (bResult)
			break;
	}
	::DeleteObject(rgnMain);
	return bResult;
#else
	return false;
#endif
}

void BasePopupDialog::rePosWnd()
{
#ifdef Q_OS_WIN
	int screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);
	RECT rcMain = {};
	::GetWindowRect((HWND)getAncestorWidgetId(), &rcMain);
	int leftPos = (screenWidth - (rcMain.right - rcMain.left)) / 2;
	if (leftPos < 0)
	{
		leftPos = 0;
	}
	int topPos = (screenHeight - (rcMain.bottom - rcMain.top)) / 2;
	if (topPos < 0)
	{
		topPos = 0;
	}
	::SetWindowPos((HWND)getAncestorWidgetId(), NULL, leftPos, topPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
#endif
}

QWidget *BasePopupDialog::getAncestorWidget()
{
	QWidget *ancestorWidget = this;
	do
	{
		if (ancestorWidget->parentWidget())
			ancestorWidget = ancestorWidget->parentWidget();
		else
			return ancestorWidget;
	} while (true);
	return ancestorWidget;
}
WId BasePopupDialog::getAncestorWidgetId()
{
	QWidget *ancestorWidget = getAncestorWidget();
	if (ancestorWidget)
		return ancestorWidget->winId();
	return winId();
}

#ifdef Q_OS_WIN
BOOL BasePopupDialog::MonitorEnumProc(HMONITOR hMonitor,
	HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	Q_UNUSED(hdcMonitor);
	Q_UNUSED(lprcMonitor);
	BasePopupDialog *_this = (BasePopupDialog *)(dwData);
	//std::vector<RECT>* list = (std::vector<RECT>*)(dwData);
	//保存显示器信息
	MONITORINFO monitorinfo;
	monitorinfo.cbSize = sizeof(MONITORINFO);
	//获得显示器信息，将信息保存到monitorinfo中
	GetMonitorInfo(hMonitor, &monitorinfo);
	//若检测到主屏
	if (monitorinfo.dwFlags == MONITORINFOF_PRIMARY)
	{
		if (_this->m_bFirstCheckMainMonitor)  //第一次检测到主屏
		{
			_this->m_bFirstCheckMainMonitor = FALSE;
			_this->m_listMonitorRect.push_back(monitorinfo.rcMonitor);
			return TRUE;
		}
		else //第二次检测到主屏,说明所有的监视器都已经检测了一遍，故可以停止检测了
		{
			_this->m_bFirstCheckMainMonitor = TRUE;    //标志复位
			return FALSE;    //结束检测
		}
	}
	_this->m_listMonitorRect.push_back(monitorinfo.rcMonitor);
	return TRUE;
}
#endif

bool BasePopupDialog::eventFilter(QObject *watched, QEvent *event)
{
#ifdef Q_OS_WIN
	if (watched == this)
	{
		//窗口停用，变为不活动的窗口  
		if (QEvent::WindowActivate == event->type())
		{
#ifdef Q_OS_WIN
			if (!wndIsVisible())
			{
				rePosWnd();
			}
#endif // Q_OS_WIN
		}
	}
#endif
	return QDialog::eventFilter(watched, event);
}
bool BasePopupDialog::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
#ifdef Q_OS_WIN
	MSG* msg = reinterpret_cast<MSG*>(message);
	switch (msg->message)
	{
	case WM_DESTROY:
	{
		if (msg->hwnd == (HWND)winId())
		{
			close();
		}
		break;
	}
	break;
	default:
		break;
	}
#endif
	return QDialog::nativeEvent(eventType, message, result);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//BasePopupWidget
BasePopupWidget::BasePopupWidget(QWidget* parent)
	: QWidget(parent)
	, m_bFirstCheckMainMonitor(false)
	, m_resizeObject(nullptr)
{
	installEventFilter(this);
}

BasePopupWidget::~BasePopupWidget()
{
}

void BasePopupWidget::setResizable(bool b)
{
	if (m_resizeObject == nullptr)
		m_resizeObject = new WindowStyle(this);
	m_resizeObject->activateOn(this, b);
}

void BasePopupWidget::setPopupFlags(bool b)
{
	setWindowFlag(Qt::Dialog, b);
	setWindowFlag(Qt::FramelessWindowHint, b);
}

bool BasePopupWidget::wndIsVisible()
{
#ifdef Q_OS_WIN
	// 过滤这种窗口隐藏的情况
	if (!::IsWindowVisible((HWND)getAncestorWidgetId()))
		return true;
	m_listMonitorRect.clear();
	m_bFirstCheckMainMonitor = TRUE;
	::EnumDisplayMonitors(NULL, NULL, BasePopupWidget::MonitorEnumProc, (LPARAM)this);
	if (m_listMonitorRect.size() == 0)
		return true;
	bool bResult = false;
	RECT rcMain = {};
	::GetWindowRect((HWND)getAncestorWidgetId(), &rcMain);
	HRGN rgnMain = ::CreateRectRgn(rcMain.left, rcMain.top, rcMain.right, rcMain.bottom);
	RECT rcTaskBar = {};
	::GetWindowRect(::FindWindow(TEXT("Shell_TrayWNd"), NULL), &rcTaskBar);
	foreach(RECT rc, m_listMonitorRect)
	{
		// 过滤任务栏的情况
		RECT rcCurTaskBar = { rc.left, rcTaskBar.top, rc.right, rcTaskBar.bottom };
		HRGN rgnTaskBar = ::CreateRectRgn(rcCurTaskBar.left, rcCurTaskBar.top, rcCurTaskBar.right, rcCurTaskBar.bottom);
		HRGN rgnMonitor = ::CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
		::CombineRgn(rgnMonitor, rgnMonitor, rgnTaskBar, RGN_XOR);
		::CombineRgn(rgnMonitor, rgnMain, rgnMonitor, RGN_AND);
		RECT rcCombine = {};
		if (::GetRgnBox(rgnMonitor, &rcCombine) == SIMPLEREGION)
		{
			bResult = true;
		}
		::DeleteObject(rgnMonitor);
		::DeleteObject(rgnTaskBar);
		if (bResult)
			break;
	}
	::DeleteObject(rgnMain);
	return bResult;
#else
	return false;
#endif
}
void BasePopupWidget::rePosWnd()
{
#ifdef Q_OS_WIN
	int screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);
	RECT rcMain = {};
	::GetWindowRect((HWND)getAncestorWidgetId(), &rcMain);
	int leftPos = (screenWidth - (rcMain.right - rcMain.left)) / 2;
	if (leftPos < 0)
	{
		leftPos = 0;
	}
	int topPos = (screenHeight - (rcMain.bottom - rcMain.top)) / 2;
	if (topPos < 0)
	{
		topPos = 0;
	}
	::SetWindowPos((HWND)getAncestorWidgetId(), NULL, leftPos, topPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
#endif
}
QWidget *BasePopupWidget::getAncestorWidget()
{
	QWidget *ancestorWidget = this;
	do
	{
		if (ancestorWidget->parentWidget())
			ancestorWidget = ancestorWidget->parentWidget();
		else
			return ancestorWidget;
	} while (true);
	return ancestorWidget;
}
WId BasePopupWidget::getAncestorWidgetId()
{
	QWidget *ancestorWidget = getAncestorWidget();
	if (ancestorWidget)
		return ancestorWidget->winId();
	return winId();
}
#ifdef Q_OS_WIN
BOOL BasePopupWidget::MonitorEnumProc(HMONITOR hMonitor,
	HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	Q_UNUSED(hdcMonitor);
	Q_UNUSED(lprcMonitor);
	BasePopupWidget *_this = (BasePopupWidget *)(dwData);
	//std::vector<RECT>* list = (std::vector<RECT>*)(dwData);
	//保存显示器信息
	MONITORINFO monitorinfo;
	monitorinfo.cbSize = sizeof(MONITORINFO);
	//获得显示器信息，将信息保存到monitorinfo中
	GetMonitorInfo(hMonitor, &monitorinfo);
	//若检测到主屏
	if (monitorinfo.dwFlags == MONITORINFOF_PRIMARY)
	{
		if (_this->m_bFirstCheckMainMonitor)  //第一次检测到主屏
		{
			_this->m_bFirstCheckMainMonitor = FALSE;
			_this->m_listMonitorRect.push_back(monitorinfo.rcMonitor);
			return TRUE;
		}
		else //第二次检测到主屏,说明所有的监视器都已经检测了一遍，故可以停止检测了
		{
			_this->m_bFirstCheckMainMonitor = TRUE;    //标志复位
			return FALSE;    //结束检测
		}
	}
	_this->m_listMonitorRect.push_back(monitorinfo.rcMonitor);
	return TRUE;
}
#endif
bool BasePopupWidget::eventFilter(QObject *watched, QEvent *event)
{
#ifdef Q_OS_WIN
	if (watched == this)
	{
		//窗口停用，变为不活动的窗口  
		if (QEvent::WindowActivate == event->type())
		{
#ifdef Q_OS_WIN
			if (!wndIsVisible())
			{
				rePosWnd();
			}
#endif // Q_OS_WIN
		}
	}
#endif
	return QWidget::eventFilter(watched, event);
}
bool BasePopupWidget::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
#ifdef Q_OS_WIN
	MSG* msg = reinterpret_cast<MSG*>(message);
	switch (msg->message)
	{
	case WM_DESTROY:
	{
		if (msg->hwnd == (HWND)winId())
		{
			close();
		}
		break;
	}
	break;
	default:
		break;
	}
#endif
	return QWidget::nativeEvent(eventType, message, result);
}
