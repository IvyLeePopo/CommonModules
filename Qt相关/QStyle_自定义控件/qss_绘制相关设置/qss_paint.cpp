#include "qss_paint.h"
#include "../../qt_pch/pch.h"

void qssNotWork()
{
	//1������ virtual paintEvent()��
	void paintEvent(QPaintEvent *event)
	{
		// ����ԭ������
		QStyleOption opt;
		opt.init(this);
		QPainter p(this);
		style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
	}

	//2�������QWidget���Ƕ���widget(����������Ϊ���ڵ���һ��)��ʱ��
	//���Կ��ǲ��̳�QWidget��ת���̳� QFrame��
	//��QFrame����������ʽ������Ļ���������д������paintEvent������
}

void paintIconInPaintEvent()
{
	// ���Ƶ�ͼ��
	void paintEvent(QPaintEvent * event)
	{
		QPainter p(this);
		QRect r = rect();
		QPixmap icon(imageutils::getSvgResPathNomal("saveAs"));//SVG��Ƭ
		QRect iconR(r.x() + (r.width() - _S(12)) / 2, r.y(), _S(12), r.height());
		p.drawPixmap(iconR, icon);
	}
}

//����qss�ļ���·��
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
	//���Ʊ߿���������
	void paintEvent(QPaintEvent *e)
	{
		QPainter p(this);
		p.setRenderHint(QPainter::Antialiasing, true);
		p.setBrush(QColor(m_bgColor));
		QPen pen;
		pen.setColor(borderColor.isEmpty() ? Qt::transparent : QColor(borderColor));
		pen.setWidth(_S(1));
		p.setPen(pen);
		//������Ʒ�ʽ���߿�᲻����
		//p.drawRoundedRect(rect(), _S(19), _S(19));
		//ԭ��rect = QRect{0,0,60,32}����>adjusted:Qrect{1,1,58,30}
		//��������һȦ
		p.drawRoundedRect(rect().adjusted(pen.width(), pen.width(), -pen.width(), -pen.width()), height() / 2, height() / 2);
	}
}

void reLayout()
{
	ui->central->layout()->removeWidget(ui->btnMinus);
	(qobject_cast<QHBoxLayout*>ui->central->layout())->insertWidget(0, ui->btnMinus);

	//((QHBoxLayout*)ui->central->layout())->insertWidget(0,ui->btnMinus);
}
