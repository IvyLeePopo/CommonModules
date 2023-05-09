#include "navwidgettips.h"
/**
	实例演示：包含其头文件，直接用
*/

//头文件
class NavWidgetTp;
void showNameTip(bool visable);
virtual bool eventFilter(QObject *watch, QEvent *event);//主要是这个函数
QPointer<NavWidgetTp> m_pImageNameTips;




