#include "qss_paint.h"
#include "../../qt_pch/pch.h"

void qssNotWork()
{
	//1、重载 virtual paintEvent()：
	void paintEvent(QPaintEvent *event)
	{
		// 绘制原风格界面
		QStyleOption opt;
		opt.init(this);
		QPainter p(this);
		style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
	}

	//2、如果此QWidget不是顶层widget(即将被绘制为窗口的那一层)的时候，
	//可以考虑不继承QWidget，转而继承 QFrame，
	//对QFrame进行设置样式表操作的话，无需重写上述的paintEvent函数；
}

void paintIconInPaintEvent()
{
	// 绘制的图标
	void paintEvent(QPaintEvent * event)
	{
		QPainter p(this);
		QRect r = rect();
		QPixmap icon(imageutils::getSvgResPathNomal("saveAs"));//SVG照片
		QRect iconR(r.x() + (r.width() - _S(12)) / 2, r.y(), _S(12), r.height());
		p.drawPixmap(iconR, icon);
	}
}

//加载qss文件的路径
void setWidgetQss(QWidget * widget, const QString & strQssPath)
{
	QFile f(strQssPath);
	if (!f.open(QIODevice::ReadOnly))
	{
		return;
	}

	widget->setStyleSheet("");
	widget->setStyleSheet(f.readAll());
	f.close();
}

void paintBoder()
{
	//绘制边框不连续问题
	void paintEvent(QPaintEvent *e)
	{
		QPainter p(this);
		p.setRenderHint(QPainter::Antialiasing, true);
		p.setBrush(QColor(m_bgColor));
		QPen pen;
		pen.setColor(borderColor.isEmpty() ? Qt::transparent : QColor(borderColor));
		pen.setWidth(_S(1));
		p.setPen(pen);
		//这个绘制方式，边框会不连续
		//p.drawRoundedRect(rect(), _S(19), _S(19));
		//原本rect = QRect{0,0,60,32}——>adjusted:Qrect{1,1,58,30}
		//往里面缩一圈
		p.drawRoundedRect(rect().adjusted(pen.width(), pen.width(), -pen.width(), -pen.width()), height() / 2, height() / 2);
	}
}

void reLayout()
{
	ui->central->layout()->removeWidget(ui->btnMinus);
	(qobject_cast<QHBoxLayout*>ui->central->layout())->insertWidget(0, ui->btnMinus);

	//((QHBoxLayout*)ui->central->layout())->insertWidget(0,ui->btnMinus);
}
