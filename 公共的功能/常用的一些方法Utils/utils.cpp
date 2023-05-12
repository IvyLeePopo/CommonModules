#include "utils.h"

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


bool readSharedFileMap(const char* pszDataKey, string& strData)
{
	if (nullptr == pszDataKey)
		return false;

	HANDLE pHandle = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, pszDataKey);
	if (nullptr == pHandle)
		return false;

	//CloseHandle(pHandle);

	//pHandle = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, pszDataKey);
	//if (nullptr == pHandle)
	//	return false;

	LPTSTR lpMapAddr = (LPTSTR)MapViewOfFile(pHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (NULL == lpMapAddr)
		return false;

	strData = "";
	strData = (const char*)lpMapAddr;

	memset(lpMapAddr, 0, strData.size());
	UnmapViewOfFile(lpMapAddr);
	CloseHandle(pHandle);
	pHandle = NULL;
	return !strData.empty();
}

bool clearSharedFileMap(const char* pszDataKey)
{
	if (nullptr == pszDataKey)
		return false;

	HANDLE pHandle = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, pszDataKey);
	if (nullptr == pHandle)
		return false;

	LPTSTR lpMapAddr = (LPTSTR)MapViewOfFile(pHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (NULL == lpMapAddr)
		return false;

	string strData = (const char*)lpMapAddr;
	memset(lpMapAddr, 0, strData.size());
	UnmapViewOfFile(lpMapAddr);
	CloseHandle(pHandle);
	pHandle = NULL;
	return true;
}

int getRandValue()
{
	int nMin = 1000;
	int nMax = 1000000;
	int nTime = (int)getCurrentTimeStamp() / 1000000;
	int nValue = nMin + rand() % (nMax - nMin + 1);
	return nValue + nTime;
}