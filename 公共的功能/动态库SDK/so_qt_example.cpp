#include "so_qt_example.h"

void loadDll()
{
	QString strLibFile = "/opt/scaner/lib/libICC_GV.so"; //金益读卡器库
	m_libReader = new QLibrary(strLibFile);

	if (!m_libReader->load())
		qDebug() << __LINE__ << __FUNCTION__ << "load fail";
	else
		qDebug() << __LINE__ << __FUNCTION__ << "load success";

	JT_OpenReader = (TYPE_JT_OpenReader)m_libReader->resolve("JT_OpenReader");

	if (!JT_OpenReader)
		qDebug() << __LINE__ << __FUNCTION__ << "JT_OpenReader not resolve";
	else
		qDebug() << __LINE__ << __FUNCTION__ << "JT_OpenReader success ";


	int handle1 = JT_OpenReader(0, com1.toLocal8Bit().data());//打开读卡器，获取句柄
}