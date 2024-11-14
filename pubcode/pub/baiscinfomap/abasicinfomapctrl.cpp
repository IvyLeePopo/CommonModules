#include "../pubcode/pub/baiscinfomap/abasicinfomapctrl.h"

QString ABasicinfoMapCtrl::queryStationNameByStationHex(const QString sStationHex)
{
    if(m_pBaiscInfoMap)
        return m_pBaiscInfoMap->queryStationNameByStationHex(sStationHex);
    else
        return "";
}

QString ABasicinfoMapCtrl::queryStationIdByStationHex(const QString sStationHex)
{
    if(m_pBaiscInfoMap)
        return m_pBaiscInfoMap->queryStationIdByStationHex(sStationHex);
    else
        return "";
}

bool ABasicinfoMapCtrl::queryStationNameByPartName(const QString sPartName, QList<QVariantMap> &qlVmap)
{
    if(m_pBaiscInfoMap)
        return m_pBaiscInfoMap->queryStationNameByPartName(sPartName, qlVmap);
    else
        return false;
}

bool ABasicinfoMapCtrl::queryStationNameByPinYing(const QString sPinYing, QList<QVariantMap> &qlVmap, int nProvinceCode)
{
    if(m_pBaiscInfoMap)
        return m_pBaiscInfoMap->queryStationNameByPinYing(sPinYing, qlVmap, nProvinceCode);
    else
        return false;
}

bool ABasicinfoMapCtrl::queryStationInfoByProvinceCode(QList<QVariantMap> &qlVmap, int nProvinceCode)
{
    if(m_pBaiscInfoMap)
        return m_pBaiscInfoMap->queryStationInfoByProvinceCode(qlVmap, nProvinceCode);
    else
        return false;
}

bool ABasicinfoMapCtrl::updateBasicinfoMap(const QByteArray baParams,bool bOnlyUpdateCache)
{
    if(m_pBaiscInfoMap)
        return m_pBaiscInfoMap->updateBasicinfoMap(baParams,bOnlyUpdateCache);
    else
        return false;
}

ABasicinfoMapCtrl::ABasicinfoMapCtrl()
{
    m_pBaiscInfoMap = new ABaiscInfoMap();
}

ABasicinfoMapCtrl::~ABasicinfoMapCtrl()
{
    if(m_pBaiscInfoMap)
        delete m_pBaiscInfoMap;
    m_pBaiscInfoMap = nullptr;
}
