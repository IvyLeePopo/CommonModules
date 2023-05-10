
#include "pch.h"
#include "shadowborder.h"
#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QPainter>
#include <qglobal.h>
#include "common/dpiscaled.h"
#include "shadowborderfunc.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

ShadowBorder::ShadowBorder(QWidget *hostWidget, QWidget *parent,
	bool bNeedResize, int nThickness)
	: QDialog(nullptr, Qt::Tool | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint)
	, m_hoverType(SZ_NOTHING)
	, m_pressedType(SZ_NOTHING)
	, m_host(hostWidget)
	, m_bNeedResize(bNeedResize)
	, m_bChangeZOrder(true)
	, m_borderThickness(qMax<int>(0, nThickness))
	, m_fColMutiplier(2.4f)
{
	setProperty("winClassName", "QDialog");
	setProperty("BorderWidget", true);
	setVisible(false);
	setAttribute(Qt::WA_TranslucentBackground);
	setMouseTracking(true);
	setFocusPolicy(Qt::NoFocus);
	m_delayedInitShowTimer.setSingleShot(true);
	connect(&m_delayedInitShowTimer, &QTimer::timeout, this, &ShadowBorder::onDelayedShowTimer);
	m_makesureShowBorder.setSingleShot(true);
	connect(&m_makesureShowBorder, &QTimer::timeout, this, &ShadowBorder::onMakesureBorderShowTimer);
	if (m_host)
		m_host->installEventFilter(this);
}

void ShadowBorder::setNeedResize(bool b)
{
	m_bNeedResize = b;
	m_hoverType = SZ_NOTHING;
	setCursor(Qt::ArrowCursor);
}

void ShadowBorder::setChangeZOrder(bool value)
{
	m_bChangeZOrder = value;
}

void ShadowBorder::setColorMultiplier(float fMultiplier)
{
	if (fMultiplier <= 0)
		fMultiplier = 1.0f;
	m_fColMutiplier = fMultiplier;
}

void ShadowBorder::reset()
{
	delayHandleShowEvent();
}

bool ShadowBorder::eventFilter(QObject *obj, QEvent *e)
{
	if (obj && obj == m_host)
	{
		switch (e->type())
		{
		case QEvent::Show:
			delayHandleShowEvent();
			break;
		case QEvent::Hide:
			hide();
			break;
		case QEvent::Resize:
		case QEvent::Move:
			resetGeometry();
			break;
		case QEvent::WindowActivate:
		case QEvent::WindowDeactivate:
			::SetWindowPos((HWND)winId(), (HWND)m_host->winId(), 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
			update();
			break;
		case QEvent::Destroy:
			destroy();
			break;
		}
	}
	return QWidget::eventFilter(obj, e);
}

void ShadowBorder::paintEvent(QPaintEvent *e)
{
	QPainter painter(this);
	shadowborderfunc::_drawBorder(&painter, rect(), m_borderThickness, m_fColMutiplier);
}

void ShadowBorder::leaveEvent(QEvent *event)
{
	unsetCursor();
}

QWidget* ShadowBorder::host() const
{
	return m_host;
}

int ShadowBorder::borderThickness() const
{
	qreal scale = QApplication::desktop()->devicePixelRatio();
	return m_borderThickness * scale;
}

void ShadowBorder::setBorderThickness(int nThickness)
{
	m_borderThickness = qMax<int>(0, nThickness);
}

void ShadowBorder::delayHandleShowEvent()
{
	if (!m_delayedInitShowTimer.isActive())
	{
		m_delayedInitShowTimer.start(1);
	}
}

void ShadowBorder::onDelayedShowTimer()
{
	resetGeometry();
	makesureBorderShow();
}

void ShadowBorder::onMakesureBorderShowTimer()
{
	if (!m_host.isNull())
		::SetWindowPos((HWND)winId(), (HWND)m_host->winId(), 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
}

bool ShadowBorder::onWndNCHittest(MSG *msg, long *result)
{
	if (!m_bNeedResize)
		return false;
	HWND hWnd = (HWND)m_host->effectiveWinId();
	POINT point = { LOWORD(msg->lParam), HIWORD(msg->lParam) };
	RECT rect = {};
	::GetWindowRect(hWnd, &rect);
	// 限制拉伸图标出现的位置
	int dragBorderWidth = _S(DRAG_BORDER_WIDTH);
	RECT rcIner = rect;
	RECT rcOuter = { rect.left - dragBorderWidth, rect.top - dragBorderWidth, rect.right + dragBorderWidth, rect.bottom + dragBorderWidth };
	HRGN rgnIner = ::CreateRectRgn(rcIner.left, rcIner.top, rcIner.right, rcIner.bottom);
	HRGN rgnOuter = ::CreateRectRgn(rcOuter.left, rcOuter.top, rcOuter.right, rcOuter.bottom);
	::CombineRgn(rgnOuter, rgnOuter, rgnIner, RGN_XOR);
	if (!::PtInRegion(rgnOuter, point.x, point.y))
	{
		::DeleteObject(rgnOuter);
		::DeleteObject(rgnIner);
		return false;
	}
	::DeleteObject(rgnOuter);
	::DeleteObject(rgnIner);
	if (point.x <= rect.left + dragBorderWidth
		&& point.y <= rect.top + dragBorderWidth)
	{
		*result = HTTOPLEFT;
		return true;
	}
	else if (point.x >= rect.right - dragBorderWidth
		&& point.y <= rect.top + dragBorderWidth)
	{
		*result = HTTOPRIGHT;
		return true;
	}
	else if (point.x <= rect.left + dragBorderWidth
		&& point.y >= rect.bottom - dragBorderWidth)
	{
		*result = HTBOTTOMLEFT;
		return true;
	}
	else if (point.x >= rect.right - dragBorderWidth
		&& point.y >= rect.bottom - dragBorderWidth)
	{
		*result = HTBOTTOMRIGHT;
		return true;
	}
	else if (point.x <= rect.left + dragBorderWidth)
	{
		*result = HTLEFT;
		return true;
	}
	else if (point.x >= rect.right)
	{
		*result = HTRIGHT;
		return true;
	}
	else if (point.y <= rect.top + dragBorderWidth)
	{
		*result = HTTOP;
		return true;
	}
	else if (point.y >= rect.bottom)
	{
		*result = HTBOTTOM;
		return true;
	}
	return false;
}

bool ShadowBorder::onWndNCLButtonDown(MSG *msg, long *result)
{
	if (!m_bNeedResize)
		return false;
	HWND hWnd = (HWND)m_host->effectiveWinId();
	RECT rect = {};
	::GetWindowRect(hWnd, &rect);
	POINT point = { LOWORD(msg->lParam), HIWORD(msg->lParam) };
	switch (msg->wParam)
	{    // nHitTest==HTCAPTION 不用判断直接调用缺省即可
	case HTTOP:
		::SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, FALSE, NULL, 0);
		::SendMessageW(hWnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_TOP, MAKELPARAM(point.x, point.y));
		::SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, TRUE, NULL, 0);
		return true;
	case HTBOTTOM:
		::SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, FALSE, NULL, 0);
		::SendMessageW(hWnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOM, MAKELPARAM(point.x, point.y));
		::SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, TRUE, NULL, 0);
		return true;
	case HTLEFT:
		::SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, FALSE, NULL, 0);
		::SendMessageW(hWnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_LEFT, MAKELPARAM(point.x, point.y));
		::SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, TRUE, NULL, 0);
		return true;
	case HTRIGHT:
		::SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, FALSE, NULL, 0);
		::SendMessageW(hWnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_RIGHT, MAKELPARAM(point.x, point.y));
		::SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, TRUE, NULL, 0);
		return true;
	case HTTOPLEFT:
		::SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, FALSE, NULL, 0);
		::SendMessageW(hWnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_TOPLEFT, MAKELPARAM(point.x, point.y));
		::SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, TRUE, NULL, 0);
		return true;
	case HTTOPRIGHT:
		::SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, FALSE, NULL, 0);
		::SendMessageW(hWnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_TOPRIGHT, MAKELPARAM(point.x, point.y));
		::SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, TRUE, NULL, 0);
		return true;
	case HTBOTTOMLEFT:
		::SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, FALSE, NULL, 0);
		::SendMessageW(hWnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOMLEFT, MAKELPARAM(point.x, point.y));
		::SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, TRUE, NULL, 0);
		return true;
	case HTBOTTOMRIGHT:
		::SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, FALSE, NULL, 0);
		::SendMessageW(hWnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOMRIGHT, MAKELPARAM(point.x, point.y));
		::SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, TRUE, NULL, 0);
		return true;
	}
	return false;
}

bool ShadowBorder::isNeedHide() const
{
	if (m_host)
	{
		if (m_host->isFullScreen() || m_host->isMaximized() || m_host->isMinimized())
		{
			return true;
		}
	}
	return false;
}

void ShadowBorder::makesureBorderShow()
{
	HWND hMainWnd = ::GetNextWindow((HWND)winId(), GW_HWNDPREV);
	if (!m_host.isNull())
	{
		if (hMainWnd != (HWND)m_host->winId())
		{
			if (!m_makesureShowBorder.isActive())
			{
				m_makesureShowBorder.start(1000);
			}
		}
	}

}

void ShadowBorder::resetGeometry()
{
	if (!m_host)
		return;
	if (isNeedHide())
	{
		hide();
	}
	else if (m_host->isVisible())
	{
		if (isHidden() && !m_delayedInitShowTimer.isActive())
		{
			show();
			if (m_bChangeZOrder)
			{
				lower();
				m_host->raise();
			}
		}
		QRect geom = m_host->geometry();
		qreal scale = QApplication::desktop()->devicePixelRatio();
		geom = QRect(geom.x() * scale, geom.y() * scale, geom.width() * scale, geom.height() * scale);
#ifdef Q_OS_WIN
		::MoveWindow((HWND)winId(), geom.left() - borderThickness(),
			geom.top() - borderThickness(),
			geom.width() + 2 * borderThickness(),
			geom.height() + 2 * borderThickness(),
			false);
#else
		setGeometry(
			geom.left() - borderThickness(),
			geom.top() - borderThickness(),
			geom.width() + 2 * borderThickness(),
			geom.height() + 2 * borderThickness());
#endif
		::SetWindowPos((HWND)winId(), (HWND)m_host->winId(), 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
	}
}

#ifdef Q_OS_WIN
bool ShadowBorder::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
	Q_UNUSED(eventType);
	MSG* msg = reinterpret_cast<MSG*>(message);
	bool ret = winEvent(msg, result);
	if (ret)
		return true;
	else
		return QWidget::nativeEvent(eventType, message, result);
}

bool ShadowBorder::winEvent(MSG *msg, long *result)
{
	Q_UNUSED(result);
	if (m_bNeedResize)
	{
		if (msg->message == WM_CAPTURECHANGED)
		{
			::PostMessage((HWND)effectiveWinId(), WM_LBUTTONUP, 0, MAKELPARAM(x(), y()));
		}
		else if (msg->message == WM_ACTIVATE)
		{
			return true;
		}
		else if (msg->message == WM_NCHITTEST)
		{
			return onWndNCHittest(msg, result);
		}
		else if (msg->message == WM_NCLBUTTONDOWN)
		{
			return onWndNCLButtonDown(msg, result);
		}
	}
	return false;
}

void ShadowBorder::changeEvent(QEvent *event)
{
	if (this->windowState() == Qt::WindowMinimized)
	{
		resetGeometry();
	}
	QWidget::changeEvent(event);
}
#endif
