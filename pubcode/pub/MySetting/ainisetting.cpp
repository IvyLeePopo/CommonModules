#include "ainisetting.h"

#include <QFile>
#include <QDebug>
#define Pos_Not_Found	-1
//------------------------------------------------------------------
AIniSetting::AIniSetting(QString sIniFile) : QObject(nullptr)
{
	m_sCfgFile = sIniFile;

	LoadCfgFromINI();
}
//------------------------------------------------------------------
AIniSetting::~AIniSetting()
{

}
//------------------------------------------------------------------
void AIniSetting::LoadCfgFromINI()
{
	QSettings aCfgSetting(m_sCfgFile,QSettings::IniFormat);
    aCfgSetting.setIniCodec("utf-8");

	m_slSections = aCfgSetting.childGroups();
	QStringList keys = aCfgSetting.allKeys();
	for(int i=0;i<keys.size();i++)
	{
		QString sKey = keys.at(i);
		//对逗号的处理有问题，用 myToString
		//QString sKeyData = aCfgSetting.value(sKey,"").toString();
		QString sKeyData = myToString(aCfgSetting.value(sKey,""));

		if(sKeyData.isEmpty())
			continue;

        int iPos = sKeyData.indexOf("//");
        int iLastPos = sKeyData.lastIndexOf("//");
        bool bSpaPos = false;
        if(iPos>2)
        {
           //处理http://或https://或ws://这种特殊情况
           if(QString::fromLocal8Bit("http:") == sKeyData.mid(iPos-5,5))
               bSpaPos = true;
           else if(iPos>2 && QString::fromLocal8Bit("ws:") == sKeyData.mid(iPos-3,3))
               bSpaPos = true;
           else if(iPos>5 && QString::fromLocal8Bit("https:") == sKeyData.mid(iPos-6,6))
               bSpaPos = true;
        }
        if(iPos>=0 && false == bSpaPos)
		{
			m_qmKeyValue[sKey.toLower()] = sKeyData.left(iPos).trimmed();
			m_qmKeyComment[sKey] = sKeyData.mid(iPos+2);
		}
		else
		{
            if(iPos == iLastPos)
                m_qmKeyValue[sKey.toLower()] = sKeyData.trimmed();
            else
            {
                m_qmKeyValue[sKey.toLower()] = sKeyData.left(iLastPos).trimmed();//处理https://后面还带有//注释的问题
                m_qmKeyComment[sKey] = sKeyData.mid(iLastPos+2);
            }
		}
		m_qmSensitiveKey[sKey.toLower()] = sKey;
	}
}
//------------------------------------------------------------------
QString AIniSetting::myToString(const QVariant &value)
{
	QString string;
	if (value.type() == QVariant::StringList)
	  string = value.toStringList().join(",");
	else
	  string = value.toString();

	return string;
}
//------------------------------------------------------------------
QVariant AIniSetting::value(const QString & sKey,const QVariant & sDefValue)
{
	QString sKeyLower = sKey.toLower();
	if(m_qmKeyValue.count(sKeyLower)<=0)
		return sDefValue;

	return m_qmKeyValue[sKeyLower];
}
//------------------------------------------------------------------
QString AIniSetting::getCfgValue(const QString & section,const QString & key,const QString def)
{
	QString sSectionKey = section+"/"+key;
	sSectionKey = sSectionKey.toLower();

	if(m_qmKeyValue.count(sSectionKey)<=0) return def;
	return m_qmKeyValue[sSectionKey].toString();
}

int AIniSetting::getCfgValue(const QString &section, const QString &key, const int def)
{
    return getCfgValue(section, key, QString::number(def)).toInt();
}

double AIniSetting::getCfgValue(const QString &section, const QString &key, const double def)
{
    return getCfgValue(section, key, QString::number(def)).toDouble();
}

QString AIniSetting::getCfgComment(const QString &section, const QString &key, const QString def)
{
	QString sSectionKey = section+"/"+key;

	if(m_qmKeyComment.count(sSectionKey)<=0) return def;
	return m_qmKeyComment[sSectionKey];
}
//------------------------------------------------------------------
bool AIniSetting::setCfgValue(const QString &section, const QString &key,const QString &value,const QString sComment)
{
    QString sSectionKey = section + "/" + key;
    m_qmKeyValue[sSectionKey.toLower()] = value;
    m_qmSensitiveKey[sSectionKey.toLower()] = sSectionKey;
    if(sComment.length()>0)
    {
        m_qmKeyComment[sSectionKey] = "//"+sComment;
    }

    //同步到文件中,不使用 m_pCfgSetting->setValue();
    saveCfgValue(section,key,value,sComment);

    if(m_slSections.contains(section,Qt::CaseInsensitive)==false)
        m_slSections.push_back(section);

    return true;
}
//--------------------------------------------------------------------------
/*!
 * \brief 删除片段配置
 * \param section 片段名
 */
void AIniSetting::delSection(const QString &section)
{
	if(section.length()<=0)
		return;

	QString sectionLower = section.toLower();
	QStringList keyToDel, keyToDelLower;
	for(auto it=m_qmSensitiveKey.begin(); it!=m_qmSensitiveKey.end(); ++it)
	{
		if (it.key().startsWith(sectionLower+"/"))
		{
			keyToDelLower << it.key();
			keyToDel << it.value();
		}
	}

	m_slSections.removeOne(section);
	foreach(auto key, keyToDel)
		m_qmKeyComment.remove(key);
	foreach(auto key, keyToDelLower)
	{
		m_qmKeyValue.remove(key);
		m_qmSensitiveKey.remove(key);
	}

	QString sSectionStr = "["+section+"]\r\n";

	qint64 iPos = Pos_Not_Found;
	qint64 iNextPos = Pos_Not_Found;
	if(getDeletePos(section,iPos,iNextPos)==false)
		return;

	updateIniFile(iPos,iNextPos,sSectionStr,"");
}

void AIniSetting::delSectionKey(const QString &section, const QString &key)
{
	if(section.length()<=0 || key.length()<=0)
		return;

	QString sSectionStr = "["+section+"]\r\n";

	qint64 iPos = Pos_Not_Found;
	qint64 iNextPos = Pos_Not_Found;
	if(getInsertPos(section,key,iPos,iNextPos)==false)
		return;

	updateIniFile(iPos,iNextPos,sSectionStr,"");
}
//------------------------------------------------------------------
void AIniSetting::saveCfgValue(const QString &section, const QString &key,const QString &value,const QString sComment)
{
	if(section.length()<=0 || key.length()<=0)
		return;

	QString sValueData = key + "=" + value;
	if(sComment.length()>0)
	{
		sValueData += "          //"+sComment;
	}
	sValueData += "\r\n";
	QString sSectionStr = "["+section+"]\r\n";

	qint64 iPos = -1;
	qint64 iNextPos = -1;
//	if(getInsertPos(section,key,iPos,iNextPos)==false)
//		return;

	getInsertPos(section,key,iPos,iNextPos);
	updateIniFile(iPos,iNextPos,sSectionStr,sValueData);
}
//--------------------------------------------------------------------------
/*!
 * \brief 查找更新配置的位置，找到对应key时填写非0位置，否则填写位置为0
 * \param section
 * \param key
 * \param iPos
 * \param iNextPos
 * \return
 */
bool AIniSetting::getInsertPos(const QString &section, const QString &key, qint64 &iPos, qint64 &iNextPos)
{
	QFile qfIni(m_sCfgFile);
	if(!qfIni.exists())return false;
	if(!qfIni.open(QIODevice::ReadOnly))
		return false;

	//确定key的位置
	QString sSectionStr = "["+section+"]";
	QString sKeyStr = key + "=";

	bool bToFindKey = false;
	bool bFindSection = false;
	QByteArray baLine = qfIni.readLine();
	while (baLine.length()>0)
	{
		QString sLine = baLine;
		sLine = sLine.replace(' ',"");
		sLine = sLine.replace('\t',"");

		if(false == bFindSection
		&& sLine.startsWith(sSectionStr,Qt::CaseInsensitive))
		{
			bFindSection = true;
			iPos = qfIni.pos();
			iNextPos = iPos;
			baLine = qfIni.readLine();

			bToFindKey = true;
			continue;
		}

		if(true == bToFindKey
		&& sLine.startsWith("[",Qt::CaseInsensitive))
		{
			bToFindKey = false;
			continue;
		}

		if(true == bToFindKey
		&& sLine.startsWith(sKeyStr,Qt::CaseInsensitive))
		{
			iNextPos = qfIni.pos();
			iPos = qfIni.pos() - baLine.length();
			break;
		}

		baLine = qfIni.readLine();
	}

	qfIni.close();

	if(false == bFindSection)
	{
		iPos = Pos_Not_Found;
		iNextPos = Pos_Not_Found;
	}

	return true;
}
//--------------------------------------------------------------------------
/*!
 * \brief 获取删除的位置
 * \param section
 * \param iPos	section之前
 * \param iNextPos	下个section之前
 * \return
 */
bool AIniSetting::getDeletePos(const QString &section, qint64 &iPos, qint64 &iNextPos)
{
	QFile qfIni(m_sCfgFile);
	if(!qfIni.exists())return false;
	if(!qfIni.open(QIODevice::ReadOnly))
		return false;

	//确定key的位置
	QString sSectionStr = "["+section+"]";
	bool bFindSection = false;		//已经找到section
	bool bToFindNextSection = false;

	QByteArray baLine = qfIni.readLine();
	while (baLine.length()>0)
	{
		QString sLine = QString(baLine);

		//找到section，开始查找下个section
		if(false == bFindSection
		&& sLine.startsWith(sSectionStr,Qt::CaseInsensitive))
		{
			bFindSection = true;
			iPos = qfIni.pos() - baLine.length();
			iNextPos = qfIni.pos();

			bToFindNextSection = true;
			baLine = qfIni.readLine();
			continue;
		}

		//找到下个section，中断
		if(true == bToFindNextSection)
		{
			if (sLine.startsWith("[",Qt::CaseInsensitive))
			{
				iNextPos = qfIni.pos() - baLine.length();
				break;
			}
			else
			{
				iNextPos = qfIni.pos();
			}
		}
		baLine = qfIni.readLine();
	}

	qfIni.close();

	if(false == bFindSection)
	{
		iPos = Pos_Not_Found;
		iNextPos = Pos_Not_Found;
	}

	return true;
}
//------------------------------------------------------------------
bool AIniSetting::updateIniFile(qint64 iPos, qint64 iNextPos,const QString sSection, const QString &sLine)
{
	//插入： iPos<iNextPos, iPos为插入起点，iNextPos是插入终点
	//删除： iPos>iNextPos, iNextPos为删除起点，iPos是删除终点；终点放在起点之前，中间部分被覆写产生“删除”的效果

	QFile qfIni(m_sCfgFile);
//	if(!qfIni.exists())return false;

	//在文件未增加内容
	if(iPos==Pos_Not_Found)
	{
		if(!qfIni.open(QIODevice::Append))
			return false;

		qfIni.write("\r\n",2);
		qfIni.write(sSection.toLocal8Bit().data(),sSection.toLocal8Bit().length());
		qfIni.write(sLine.toLocal8Bit().data(),sLine.toLocal8Bit().length());
		qfIni.close();
		return true;
	}

	//在文件中间增加
	if(!qfIni.open(QIODevice::ReadOnly))
		return false;

	QByteArray baIniData = qfIni.readAll();
	qfIni.close();

	if(!qfIni.open(QIODevice::WriteOnly))
		return false;

	qfIni.write(baIniData.data(),iPos);
	qfIni.write(sLine.toLocal8Bit().data(),sLine.toLocal8Bit().length());
	qfIni.write(baIniData.data()+iNextPos,baIniData.length() - iNextPos);
	qfIni.close();
	return true;
}
//------------------------------------------------------------------
int AIniSetting::getSectionCfg(QString sSection, QMap<QString, QString> &stCfgData)
{
	int iCnt = 0;
	QString sSectionKey = sSection.toLower() + "/";
	QMap<QString,QVariant>::iterator it;
	for(it = m_qmKeyValue.begin();it != m_qmKeyValue.end(); ++it)
	{
        if(it.key().startsWith(sSectionKey, Qt::CaseInsensitive))
		{
			QString sKey = m_qmSensitiveKey[it.key()];
			sKey = sKey.right(sKey.toLocal8Bit().length() - sSectionKey.toLocal8Bit().length());
			stCfgData[sKey] = it.value().toString();
			iCnt ++;
		}
	}

	return iCnt;
}
//------------------------------------------------------------------
QStringList AIniSetting::getSections()
{
	return m_slSections;
}

QString AIniSetting::getCfgFileName() const
{
	return m_sCfgFile;
}

void AIniSetting::reloadCfgFile()
{
	m_qmKeyComment.clear();
	m_qmKeyValue.clear();
	m_qmSensitiveKey.clear();
	m_slSections.clear();
    LoadCfgFromINI();
}

QString AIniSetting::getAllConfigInfo()
{
    QString sMsg;
    QMap<QString,QVariant>::iterator it = m_qmKeyValue.begin();
    for(;it != m_qmKeyValue.end();it++)
    {
        QString sKey = it.key();
        QString sValue = it.value().toString();
        QString sSensitiveGroupAndKey = m_qmSensitiveKey.value(sKey);
        QString sCommon = m_qmKeyComment.value(sSensitiveGroupAndKey);

        if(false == sMsg.isEmpty())
            sMsg += "\r\n";

        if(sCommon.isEmpty())
            sMsg += QString::fromLocal8Bit("%1=%2").arg(sSensitiveGroupAndKey).arg(sValue);
        else
            sMsg += QString::fromLocal8Bit("%1=%2, tip:%3").arg(sSensitiveGroupAndKey).arg(sValue).arg(sCommon);

    }
    return sMsg;
}
