#include "GeneralFunction.h"

void setWgtCenter()
{
	#include <QApplication>
	#include <QDesktopWidget>
	m_pChtPropertyWgt = new BLPopChartProertyWidget(GetParentWnd());
	QRect rect = QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(GetParentWnd()));
	rect.setX(rect.x() + (rect.width() - 498) / 2);
	rect.setY(rect.y() + (rect.height() - 498) / 2);
	rect.setWidth(498);
	rect.setHeight(498);
	m_pChtPropertyWgt->setGeometry(rect);
}

void calTimeGap()
{
	#include<QTime >

	QTime time;
	time.start();
	QTime::currentTime().msecsSinceStartOfDay() - (time.msecsSinceStartOfDay()) << QString("ms");
}

void repeatCode()
{
	#define TYPE_CHART_IMPL(text)                                                 \
	{                                                                             \
		auto ptmp = m_MdiArea->getCurrentWidget();                                \
		if (ptmp == nullptr) return;                                              \
		CMVChartCtrl* pChartCtrl = static_cast<CMVChartCtrl*>(ptmp->getContent());\
		pChartCtrl->OnChtMainChange(text);                                        \
		m_MdiArea->getCurrentWidget()->update();                                  \
		slotChartWndChanged();                                                    \
	}   
}


bool InitWndMutex(QRect& winRect)
{
	{
		//兼容旧版本 2020-5-25 之前
		QString sAppDirPath = QCoreApplication::applicationDirPath(); //软件路径
		CreateMutex(NULL, FALSE, sAppDirPath.toStdWString().c_str());//互斥锁
	}
	const QString sCheckKey = QCryptographicHash::hash(QCoreApplication::applicationDirPath().toLocal8Bit(), QCryptographicHash::Md5).toHex();
	const QString sMutexName = QString("[%1][%2]").arg("CT4_Client").arg(sCheckKey); //检查更新会用到这个值，不要修改
	const QString SAppName = QApplication::applicationName();
	HANDLE hMutex = CreateMutex(NULL, FALSE, sMutexName.toStdWString().c_str());
	if (hMutex && (GetLastError() == ERROR_ALREADY_EXISTS))
	{
		CloseHandle(hMutex);
		HWND hwnd = FindWindow(L"Qt5QWindowIcon", SAppName.toStdWString().c_str());
		if (nullptr == hwnd)
			hwnd = FindWindow(L"Qt5QWindowToolSaveBits", SAppName.toStdWString().c_str());
		if (nullptr != hwnd)
		{
			QWindow* widget = QWindow::fromWinId(WId(hwnd));
			if (widget != nullptr)
			{
				winRect = widget->geometry();
			}
			SetForegroundWindow(hwnd); //激活窗口
		}
		return false;
	}
	return true;
}

//是否重复启动  true 重复  false不重复
bool IsRepeatStart(const QRect & rect)
{
	QRect mRect = rect;
	if (InitWndMutex(mRect) == false)
	{
		QSettings setting(PATH_GLOBAL_CLOUD_SETTING, QSettings::IniFormat);
		QString themeType = setting.value("Setting/m_themeTopic", "E_THEMEWHITE").toString();
		BLMsgBox* msgBox;
		if (themeType == "E_THEMEWHITE") {
			msgBox = new BLMsgBox(nullptr, E_BoxType::E_WhiteBox);
		}
		else
		{
			msgBox = new BLMsgBox(nullptr, E_BoxType::E_BlackBox);
		}
		msgBox->move(mRect.x() + mRect.width() / 2 - msgBox->width() / 2, mRect.y() + mRect.height() / 2 - msgBox->height() / 2);
		msgBox->exec();
		return true;
	}
	return false;
}

void initLanguage()
{
	m_pTranslator = new QTranslator(this);
	ENUM_LANGUAGE lang = SettingsMgr::getInstance()->getLanguage();
	if (lang == LANGUAGE_CHINESE)
	{
		LOG(INFO) << "the current is chinese";
		QString mui = QString(":/zh_CN.qm");
		m_pTranslator->load(mui);
		QCoreApplication::installTranslator(m_pTranslator);
	}
	else
	{
		LOG(INFO) << "the current is English";
		QString mui = QString(":/en_US.qm");
		m_pTranslator->load(mui);
		QCoreApplication::installTranslator(m_pTranslator);
	}
}

void checkResult()
{
#define CheckResult(a)  if(!(a)) \
  return;
#define CheckResultWithRet(a, b)  if(!(a)) \
  return (b);

}
