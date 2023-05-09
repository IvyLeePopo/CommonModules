#include "point.h"

void popupPos()
{
	SdMenu* menu = new QMenu(this);//创建一个菜单
	menu->setFixedWidth(_S(100));
	QPoint pos = m_btnDownload->mapToGlobal(QPoint(0, 0));//得到该控件的左上角位置
	pos.setX(pos.x() - (menu->width() - m_btnDownload->width()) / 2);
	pos.setY(pos.y() - menu->height());
	menu->popup(pos);//菜单显示的位置
	menu->deleteLater();
}


//获得该点的绝对位置：
//将小部件坐标pos转换为全局屏幕坐标，例如，mapToGlobal(QPoint(0, 0))将给出小部件左上角像素的全局坐标。
QPoint getLeftTopPos()
{
	QPoint pos = m_btnDownload->mapToGlobal(QPoint(0, 0));//得到该控件的左上角位置
	return pos;
}

//判断当前光标是否在控件内：
//将全局屏幕坐标pos转换为小部件坐标。
bool isContain()
{
	QPoint point = mapFromGlobal(QCursor::pos());
	QRect rcDrop(m_dragStartPos.x() - dropPos, m_dragStartPos.y() - dropPos, dropPos * 2, dropPos * 2);
	if (rcDrop.contains(point))
		return true;

	return false;
}


