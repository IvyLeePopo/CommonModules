
//源文件
#include "pch.h"
#include "BaseBorderWidget.h"
#include <QEvent>
#include <QLibrary>
#include <QStyleOption>
#include <QTimer>
#include "general/genericdef.h"
#include "shadowborderfunc.h"
#ifdef Q_OS_WIN
namespace
{
	static RECT QRect2RECT(const QRect & rc)
	{
		RECT r = { rc.left(), rc.top(), rc.right(), rc.bottom() };
		return r;
	}

	static QRect RECT2QRect(const RECT & rc)
	{
		QRect r(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
		return r;
	}

	//获取任务栏的大小
	QRect getTaskManWindowRect()
	{
		typedef HWND(WINAPI *PROCGETTASKMANWND)();
		PROCGETTASKMANWND GetTaskmanWindow;
		QLibrary lib(QString::fromUtf8("user32"));
		lib.load();
		GetTaskmanWindow = (PROCGETTASKMANWND)lib.resolve("GetTaskmanWindow");
		HWND taskman = GetParent(GetTaskmanWindow());
		RECT rc;
		GetWindowRect(taskman, &rc);
		return QRect(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
	}

	QRect getMaxRectWin32(QWidget * widget)
	{
		//  HWND hwnd = widget->internalWinId();
		//  HMONITOR h = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
		//  hwnd最小化状态下,再最大化,这个时机无法取到正确的hwnd位置,
		//  所以这里使用Qt中存储的距形坐标,以修正这个问题.见BUG: 97336
		//  之所以不使用平台通用的QDesktopWidget::availableGeometry(), 是因为
		//  availableGeometry()在任务栏从一侧托到另一侧的时机中,无法取到正确的值.
		//  Qt中存储的这个值还没有刷新.
		//QRect r = widget->geometry();
		RECT rcClient = QRect2RECT(widget->geometry());
		HMONITOR h = MonitorFromRect(&rcClient, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfoW(h, &mi);
		RECT rcWork = mi.rcWork;
		QRect rcMax = RECT2QRect(rcWork);
		APPBARDATA abd;
		memset(&abd, 0, sizeof(APPBARDATA));
		abd.cbSize = sizeof(APPBARDATA);
		UINT state = (UINT)SHAppBarMessage(ABM_GETSTATE, &abd);
		if (state & ABS_AUTOHIDE)
		{
			QRect r = getTaskManWindowRect();
			if (r.bottom() >= rcMax.bottom())
				rcMax.adjust(0, 0, 0, -1);
			else if (r.left() <= rcMax.left())
				rcMax.adjust(1, 0, 0, 0);
			else if (r.right() >= rcMax.right())
				rcMax.adjust(0, 0, -1, 0);
			else if (r.top() <= rcMax.top())
				rcMax.adjust(0, 1, 0, 0);
		}
		return rcMax;
	}
} //namespace
#endif



/**************************** CustomShadowEffect **********************************/
CustomShadowEffect::CustomShadowEffect(int tickness, int radius, QWidget* attachWid, QWidget* parent /*= nullptr*/)
	:QWidget(parent)
	, m_nTickness(tickness)
	, m_nRadius(radius)
	, m_pAttachWid(attachWid)
{
	setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_ShowWithoutActivating);
	setStyleSheet("background-color:transparent;");
}
CustomShadowEffect::~CustomShadowEffect()
{
}
void CustomShadowEffect::updateGeometry()
{
	if (m_nTickness <= 0) return;
	QWidget* pTopLevelWid = m_pAttachWid->topLevelWidget();
	if (m_pAttachWid->isVisible() && pTopLevelWid->isVisible() && !isVisible())
		QTimer::singleShot(5, this, [this]() {show(); });
	else if ((!m_pAttachWid->isVisible() || !pTopLevelWid->isVisible()) && isVisible())
	{
		hide();
		return;
	}
	QPoint globalPos = m_pAttachWid->mapToGlobal(QPoint(0, 0));
	int x = globalPos.x();
	int y = globalPos.y();
	int w = m_pAttachWid->frameGeometry().width();
	int h = m_pAttachWid->frameGeometry().height();
	setGeometry(x - m_nTickness, y - m_nTickness, w + 2 * m_nTickness, h + 2 * m_nTickness);
	update();
}
void CustomShadowEffect::paintEvent(QPaintEvent *event)
{
	//clip
	QRegion rectRegion(0, 0, width(), height());
	QRegion maskRegion(m_nTickness, m_nTickness, width() - 2 * m_nTickness, height() - 2 * m_nTickness);
	setMask(rectRegion.subtracted(maskRegion));
	//shadow
	QStyleOption opt;
	opt.init(this);
	QPainter painter(this);
	shadowborderfunc::_drawBorder(&painter, rect(), m_nTickness, 2.4f, m_nRadius, QColor(44, 44, 44));
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
	QWidget::paintEvent(event);
}
/**************************** BaseBorderWidget **********************************/
BaseBorderWidget::BaseBorderWidget(int tickness, int radius, QWidget *parent)
	: BasePopupWidget(parent)
	, m_pShadowEffect(new CustomShadowEffect(tickness, radius, this, this))
{
	QWidget *pWid = topLevelWidget();
	if (pWid != this)
		pWid->installEventFilter(this);
}
BaseBorderWidget::~BaseBorderWidget()
{
}
void BaseBorderWidget::setShadowTickness(int tickness)
{
	m_pShadowEffect->setShadowTickness(tickness);
}
void BaseBorderWidget::setShadowRadius(int radius)
{
	m_pShadowEffect->setShadowRadius(radius);
}
void BaseBorderWidget::moveEvent(QMoveEvent *event)
{
	m_pShadowEffect->updateGeometry();
	QWidget::moveEvent(event);
}
void BaseBorderWidget::resizeEvent(QResizeEvent *event)
{
	m_pShadowEffect->updateGeometry();
	QWidget::resizeEvent(event);
}
bool BaseBorderWidget::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
#ifdef Q_OS_WIN
	MSG* msg = reinterpret_cast<MSG*>(message);
	static bool s_bMaximized_resize = false;
	const int WM_Maximized_Resize = WM_USER + 0x111;
	switch (msg->message)
	{
	case WM_Maximized_Resize:
	{
		s_bMaximized_resize = true;
		//进行的窗口的位置同步
		const QRect& rc = geometry();
		const int width = static_cast<int>(msg->wParam);
		const int height = static_cast<int>(msg->lParam);
		::MoveWindow((HWND)internalWinId(), rc.left() + 1, rc.top(), width, height, true);
		::MoveWindow((HWND)internalWinId(), rc.left(), rc.top(), width, height, true);
		s_bMaximized_resize = false;
	}
	break;
	case WM_WINDOWPOSCHANGING:
	{
		WINDOWPOS * pos = (WINDOWPOS *)msg->lParam;
		// 窗口最大化状态下,切换任务栏从"隐藏"到"固定",或者拖动"任务栏",均需重新计算大小.
		// 但程序并不会再收到带有SWP_STATECHANGED消息, 所以这里注释掉.
		//if (pos->flags & SWP_STATECHANGED) // 参见wine代码
		if (!(pos->flags & SWP_NOSIZE) &&
			!(pos->flags & SWP_NOMOVE))
		{
			if (s_bMaximized_resize)
				return false;
			LONG s = GetWindowLongW((HWND)internalWinId(), GWL_STYLE);
			if (s & WS_MAXIMIZE)
			{
				RECT rcWnd = { 0 };
				::GetWindowRect((HWND)internalWinId(), &rcWnd);
				QRect rc(rcWnd.left, rcWnd.top, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top);
				QRect rcMax = getMaxRectWin32(this);
				QRect rcPos(pos->x, pos->y, pos->cx, pos->cy);
				bool bMax = std::abs(rc.width() - rcMax.width()) < 2 && std::abs(rc.height() - rcMax.height()) < 2;
				bool bChangeWndSize = (rcMax.width() - rcPos.width() > 10) && (rcMax.height() - rcPos.height() < 10);
				if (bMax && bChangeWndSize)
				{
					::PostMessage((HWND)internalWinId(), WM_Maximized_Resize, pos->cx, pos->cy);
				}
				//设置大小位置
				{
					pos->x = rcMax.x();
					pos->y = rcMax.y();
					pos->cx = rcMax.width();
					pos->cy = rcMax.height();
				}
			}
		}
	}
	break;
	case WM_NCACTIVATE:
	{
		update();
		*result = TRUE;
		return true;
	}
	break;
	default:
		break;
	}
#endif
	return BasePopupWidget::nativeEvent(eventType, message, result);
}
void BaseBorderWidget::showEvent(QShowEvent *event)
{
	if (!isMinimized())
	{
		this->setAttribute(Qt::WA_Mapped);
	}
	m_pShadowEffect->updateGeometry();
	BasePopupWidget::showEvent(event);
}

void BaseBorderWidget::paintEvent(QPaintEvent *event)
{
	QStyleOption opt;
	opt.initFrom(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);//绘制样式
	BasePopupWidget::paintEvent(event);
}
bool BaseBorderWidget::eventFilter(QObject *o, QEvent *e)
{
	if (o == topLevelWidget())
	{
		switch (e->type())
		{
		case QEvent::Hide:
		case QEvent::Show:
		case QEvent::Leave:
		case QEvent::Resize:
		case QEvent::Move:
		case QEvent::LayoutRequest:
			m_pShadowEffect->updateGeometry();
			break;
		default:
			break;
		}
	}
	return BasePopupWidget::eventFilter(o, e);
}
void BaseBorderWidget::onMinimize()
{
	showMinimized();
}
void BaseBorderWidget::onMaximize()
{
	if (isMaximized())
		setWindowState(Qt::WindowNoState);
	else
		setWindowState(Qt::WindowMaximized);
}
