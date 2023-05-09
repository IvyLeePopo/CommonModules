#include "menu.h"
/**
	实例演示：包含其头文件，直接用
*/

void example()
{
	//用例
	SdMenu *menu = new SdMenu(this);
	menu->setFixedWidth(_S(150));
	QStringList m_textList;//按照实际的顺序
	m_textList
		<< tr("Colorway")
		<< tr("Material Colorway")
		<< tr("Export FBX")
		<< tr("Export OBJ")
		<< tr("Export GLB/GLTF")
		<< tr("Export USD");
	for (int i = 0; i < m_textList.size(); ++i)
	{
		QAction *pAction = menu->appendMenu(m_textList.at(i));
	}
	QPoint pos = btnWidget->mapToGlobal(QPoint(0, 0));
	QPoint posParent = btnWidget->parentWidget()->mapToGlobal(QPoint(0, 0));
	pos.setX(posParent.x() + _S(190));
	pos.setY(pos.y() + btnWidget->height() - _S(10));
	menu->popup(pos);
	menu->deleteLater();
}

