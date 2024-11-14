#include "abaiscinfomap.h"
#include <QtConcurrent>
#include "zhtopy.h"

ABaiscInfoMap::ABaiscInfoMap(QObject *parent)
    : QObject(parent),
    m_mutex(QMutex::Recursive),
    m_bIsFirstLoad(true)
{
    connect(this, SIGNAL(sigLoadBaiscInfoMapParam()),this, SLOT(sltLoadBaiscInfoMapParam()),Qt::QueuedConnection);
    connect(this, SIGNAL(sigUpdateBaiscInfoMapParam()),this, SLOT(sltUpdateBaiscInfoMapParam()),Qt::QueuedConnection);
    emit sigLoadBaiscInfoMapParam();
}

ABaiscInfoMap::~ABaiscInfoMap()
{
}

QString ABaiscInfoMap::queryStationNameByStationHex(const QString sStationHex)
{
    QMutexLocker locker(&m_mutex);
    if(m_mapStationHexAndBasicInfo.contains(sStationHex))
    {
        QVariantMap qVarMap = m_mapStationHexAndBasicInfo.value(sStationHex);
        return qVarMap.value(NS_BaiscInfoMap::sStationName).toString();
    }
    else
        return "";
}

QString ABaiscInfoMap::queryStationIdByStationHex(const QString sStationHex)
{
    QMutexLocker locker(&m_mutex);
    if(m_mapStationHexAndBasicInfo.contains(sStationHex))
    {
        QVariantMap qVarMap = m_mapStationHexAndBasicInfo.value(sStationHex);
        return qVarMap.value(NS_BaiscInfoMap::sStationId).toString();
    }
    else
        return "";
}

bool ABaiscInfoMap::queryStationNameByPartName(const QString sPartName, QList<QVariantMap> &qlVmap)
{
    bool bRet = false;
    {
        QMutexLocker locker(&m_mutex);
        QMap<QString,QVariantMap>::iterator it = m_mapStationIdAndBasicInfo.begin();
        for(;it != m_mapStationIdAndBasicInfo.end();it++)
        {
            QVariantMap qVarMap = it.value();
            QString sStationName = qVarMap.value(NS_BaiscInfoMap::sStationName).toString();
            if(sStationName.contains(sPartName))
            {
                bRet = true;
                qlVmap.append(qVarMap);
                //若匹配大于10个，则就返回前10个
                if(10 < qlVmap.size())
                    break;
            }
        }
    }
    return bRet;
}

bool ABaiscInfoMap::queryStationNameByPinYing(const QString sPinYing, QList<QVariantMap> &qlVmap, int nProvinceCode)
{
    bool bRet = false;
    if(sPinYing.isEmpty() || 3 > sPinYing.length())
    {
        return bRet;
    }

    {
        QMutexLocker locker(&m_mutex);
        QMap<QString,QVariantMap>::iterator it = m_mapStationIdAndBasicInfo.begin();
        for(;it != m_mapStationIdAndBasicInfo.end();it++)
        {
            QVariantMap qVarMap = it.value();
            QString sStationHex = qVarMap.value(NS_BaiscInfoMap::sStationHex).toString();

            QString sPinYingValue = qVarMap.value(NS_BaiscInfoMap::sStationNamePinYing).toString();
            QString sJianPinValue = qVarMap.value(NS_BaiscInfoMap::sStationNameJianPin).toString();

            if(sJianPinValue.contains(sPinYing))
            {
                //要匹配本省的入口
                if(0 < nProvinceCode && false == sStationHex.startsWith(QString::number(nProvinceCode)))
                {
                    continue;
                }

                bRet = true;
                qlVmap.append(qVarMap);
                //若匹配大于10个，则就返回前10个
                if(10 < qlVmap.size())
                    break;
            }
            else if(sPinYingValue.contains(sPinYing))
            {
                //要匹配本省的入口
                if(0 < nProvinceCode && false == sStationHex.startsWith(QString::number(nProvinceCode)))
                {
                    continue;
                }

                bRet = true;
                qlVmap.append(qVarMap);
                //若匹配大于10个，则就返回前10个
                if(10 < qlVmap.size())
                    break;
            }
        }
    }

    return bRet;
}

bool ABaiscInfoMap::queryStationInfoByProvinceCode(QList<QVariantMap> &qlVmap, int nProvinceCode)
{
    bool bRet = false;
    if(0 >= nProvinceCode)
    {
        return bRet;
    }

    {
        QMutexLocker locker(&m_mutex);
        QMap<QString,QVariantMap>::iterator it = m_mapStationIdAndBasicInfo.begin();
        for(;it != m_mapStationIdAndBasicInfo.end();it++)
        {
            QVariantMap qVarMap = it.value();
            QString sStationHex = qVarMap.value(NS_BaiscInfoMap::sStationHex).toString();

            //要匹配本省的入口
            if(sStationHex.startsWith(QString::number(nProvinceCode)))
            {
                bRet = true;
                qlVmap.append(qVarMap);
            }
        }
    }

    return bRet;
}

bool ABaiscInfoMap::updateBasicinfoMap(const QByteArray baParams,bool bOnlyUpdateCache)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(baParams, &error);
    if(error.error != QJsonParseError::NoError)
    {
        qWarning() << QString::fromUtf8("QJsonParseError:%1,Json Data:%2").arg(error.errorString()).arg(QString::fromUtf8(baParams));
        return false;
    }

    QJsonObject jsData = doc.object();
    if(false == jsData.contains("DownParamResp") || false == jsData.value("DownParamResp").isObject())
    {
        qWarning() << QString::fromUtf8("not contain key:DownParamResp");
        return false;
    }

    QJsonObject jsDownParamResp = jsData.value("DownParamResp").toObject();
    if(false == jsDownParamResp.contains("stationParam") || false == jsDownParamResp.value("stationParam").isArray())
    {
        qWarning() << QString::fromUtf8("not contain key:stationParam");
        return false;
    }

    QJsonArray jsArray = jsDownParamResp.value("stationParam").toArray();

    QMap<QString,QVariantMap> mapStationHexAndBasicInfo;
    QMap<QString,QVariantMap> mapStationIdAndBasicInfo;

    for(int i=0;i<jsArray.size();i++)
    {
        QJsonObject jsObject = jsArray.at(i).toObject();

        QString sStationName = jsObject.value(NS_BaiscInfoMap::sStationName).toString();
        QString sHex = jsObject.value(NS_BaiscInfoMap::sStationHex).toString();
        QString sStationId = jsObject.value(NS_BaiscInfoMap::sStationId).toString();

        if(sStationName.isEmpty() || sStationId.isEmpty() || sHex.isEmpty())
            continue;

        QString sPinYing = ZhToPY::Instance()->zhToPY(sStationName).replace(" ","");
        QString sJianPin = ZhToPY::Instance()->zhToJP(sStationName).toLower();

        QVariantMap qVarMap;
        qVarMap.insert(NS_BaiscInfoMap::sStationId,     sStationId);
        qVarMap.insert(NS_BaiscInfoMap::sStationName,   sStationName);
        qVarMap.insert(NS_BaiscInfoMap::sStationHex,    sHex);

        qVarMap.insert(NS_BaiscInfoMap::sStationNamePinYing,    sPinYing);
        qVarMap.insert(NS_BaiscInfoMap::sStationNameJianPin,    sJianPin);

        if(mapStationHexAndBasicInfo.contains(sHex))
        {
            QVariantMap qTmpMap = mapStationHexAndBasicInfo.value(sHex);

            if(false == bOnlyUpdateCache)
            {
                qWarning() << "重复的收费站Hex:" << sHex << "stationName:" << qTmpMap.value(NS_BaiscInfoMap::sStationName).toString();
                qWarning() << "重复的收费站Hex:" << sHex << "stationName:" << sStationName;
            }

            if(qTmpMap.value(NS_BaiscInfoMap::sStationHex).toString().contains("(弃用)"))
            {
                mapStationHexAndBasicInfo[sHex]          = qVarMap;
            }
        }
        else
            mapStationHexAndBasicInfo[sHex]          = qVarMap;

        mapStationIdAndBasicInfo[sStationId]     = qVarMap;

    }

    {
        QMutexLocker locker(&m_mutex);
        m_mapStationHexAndBasicInfo  = mapStationHexAndBasicInfo;
        m_mapStationIdAndBasicInfo   = mapStationIdAndBasicInfo;
    }

    if(mapStationIdAndBasicInfo.isEmpty())
        qWarning() << "下载的基础信息参数为空";
    else if(false == bOnlyUpdateCache)
        emit sigUpdateBaiscInfoMapParam();
    else if(true == bOnlyUpdateCache)
    {
        qInfo() << "已更新加载"<< mapStationIdAndBasicInfo.size() << "条全网基础信息参数记录";
    }
    return true;
}

void ABaiscInfoMap::sltLoadBaiscInfoMapParam()
{
    //首次启动加载基础信息映射参数表
    QtConcurrent::run([this](){
        if(false == m_bIsFirstLoad)
            return;

        QString sFile = QCoreApplication::applicationDirPath() + "/data/tbl_baiscinfomap.txt";
        QFile file;
        file.setFileName(sFile);

        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << QString::fromUtf8("failed to open %1").arg(sFile);
            return;
        }
        else
        {
            qInfo() << QString::fromUtf8("开始加载基础信息映射参数...");
            QMap<QString,QVariantMap> mapStationHexAndBasicInfo;
            QMap<QString,QVariantMap> mapStationIdAndBasicInfo;
            QMap<QString,QVariantMap> mapPinYingAndBasicInfo;

            QTextStream in(&file);
            in.setCodec("utf-8");
            int nline = 0;
            while(!in.atEnd())
            {
                nline++;
                if(1 == nline)
                    continue;

                QString sInfo = in.readLine(65535);
                QStringList slInfo = sInfo.split("	");

                //stationId	stationName	stationHex
                if(NS_BaiscInfoMap::sStationId == slInfo.at(0))
                    continue;

                if(3 <= slInfo.size())
                {
                    QString sStationName = slInfo.at(1);
                    QString sHex = slInfo.at(2);
                    QString sStationId = slInfo.at(0);

                    QString sPinYing = ZhToPY::Instance()->zhToPY(sStationName).replace(" ","");
                    QString sJianPin = ZhToPY::Instance()->zhToJP(sStationName).toLower();

//                    if(mapPinYingAndBasicInfo.contains(sPinYing))
//                    {
//                        QVariantMap qVarMapTmp = mapPinYingAndBasicInfo.value(sPinYing);
//                        qInfo() << qVarMapTmp.value(NS_BaiscInfoMap::sStationName).toString() << ":" << sPinYing << "(" << sJianPin << ")";
//                    }

                    QVariantMap qVarMap;
                    qVarMap.insert(NS_BaiscInfoMap::sStationId,     sStationId);
                    qVarMap.insert(NS_BaiscInfoMap::sStationName,   sStationName);
                    qVarMap.insert(NS_BaiscInfoMap::sStationHex,    sHex);

                    qVarMap.insert(NS_BaiscInfoMap::sStationNamePinYing,    sPinYing);
                    qVarMap.insert(NS_BaiscInfoMap::sStationNameJianPin,    sJianPin);

                    if(mapStationHexAndBasicInfo.contains(sHex))
                    {
                        QVariantMap qTmpMap = mapStationHexAndBasicInfo.value(sHex);
                        if(qTmpMap.value(NS_BaiscInfoMap::sStationHex).toString().contains("(弃用)"))
                        {
                            mapStationHexAndBasicInfo[sHex]          = qVarMap;
                        }
                    }
                    else
                        mapStationHexAndBasicInfo[sHex]          = qVarMap;

                    mapStationIdAndBasicInfo[sStationId]     = qVarMap;
                    mapPinYingAndBasicInfo[sPinYing]      = qVarMap;

                }

                if(nline%20000 == 0)
                {
                    QThread::msleep(2);
                }
            }
            file.close();
            qInfo() << QString::fromUtf8("成功加载基础信息映射参数%1条记录").arg(mapStationIdAndBasicInfo.size());
            m_bIsFirstLoad = false;

            {
                QMutexLocker locker(&m_mutex);
                m_mapStationHexAndBasicInfo  = mapStationHexAndBasicInfo;
                m_mapStationIdAndBasicInfo   = mapStationIdAndBasicInfo;
            }
        }
    });
}

void ABaiscInfoMap::sltUpdateBaiscInfoMapParam()
{
    //更新基础信息映射参数表
    QtConcurrent::run([this](){
        QString sNewFile = QCoreApplication::applicationDirPath() + "/data/tbl_baiscinfomap_recv.txt";
        QFile file;
        file.setFileName(sNewFile);

        if(!file.open(QIODevice::WriteOnly|QIODevice::Truncate))
        {
            qWarning() << QString::fromUtf8("failed to open %1").arg(sNewFile);
            return;
        }

        qInfo() << QString::fromUtf8("开始更新基础信息映射参数...");

        QTextStream out(&file);
        out.setCodec("utf-8");
        out << NS_BaiscInfoMap::sStationId
            << "	"
            << NS_BaiscInfoMap::sStationName
            << "	"
            << NS_BaiscInfoMap::sStationHex
            << endl;

        QMap<QString,QVariantMap> mapStationIdAndBasicInfo;
        {
            QMutexLocker locker(&m_mutex);
            mapStationIdAndBasicInfo = m_mapStationIdAndBasicInfo;
        }

        QMap<QString,QVariantMap>::iterator it = mapStationIdAndBasicInfo.begin();
        for(;it != mapStationIdAndBasicInfo.end();it++)
        {
            QVariantMap qVarMap = it.value();

            out << qVarMap.value(NS_BaiscInfoMap::sStationId).toString()
                << "	"
                << qVarMap.value(NS_BaiscInfoMap::sStationName).toString()
                << "	"
                << qVarMap.value(NS_BaiscInfoMap::sStationHex).toString()
                << endl;
        }
        file.close();
        qInfo() << QString::fromUtf8("成功更新基础信息映射参数%1条记录").arg(mapStationIdAndBasicInfo.size()-1);

        if(QFileInfo::exists(sNewFile))
        {
            QString sFile = QCoreApplication::applicationDirPath() + "/data/tbl_baiscinfomap.txt";
            QFileInfo fileInfo(sFile);
            if(fileInfo.isFile())
            {
                QFile fileTmp(sFile);
                if(fileTmp.open(QIODevice::ReadOnly))
                {
                    fileTmp.close();
                    if(fileTmp.remove())
                        qInfo() << QString::fromUtf8("成功删除旧参数文件：%1").arg(sFile);
                    else
                        qWarning() << QString::fromUtf8("删除旧参数文件：%1 失败").arg(sFile);
                }
            }
            QFile::rename(sNewFile,sFile);
        }

    });
}
