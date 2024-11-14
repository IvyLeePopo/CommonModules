#ifndef ABASICINFOMAPCTRL_H
#define ABASICINFOMAPCTRL_H

#include "abaiscinfomap.h"
#include "../pubcode/pub/Singleton/asingleton.h"

class ABasicinfoMapCtrl : public ASingleton<ABasicinfoMapCtrl>
{
    friend class ASingleton<ABasicinfoMapCtrl>;
public:
    QString queryStationNameByStationHex(const QString sStationHex);
    QString queryStationIdByStationHex(const QString sStationHex);
    bool queryStationNameByPartName(const QString sPartName, QList<QVariantMap> &qlVmap);
    bool queryStationNameByPinYing(const QString sPinYing, QList<QVariantMap> &qlVmap, int nProvinceCode = 0);
    bool queryStationInfoByProvinceCode(QList<QVariantMap> &qlVmap, int nProvinceCode);


    bool updateBasicinfoMap(const QByteArray baParams,bool bOnlyUpdateCache = false);
private:
    explicit ABasicinfoMapCtrl();
    virtual ~ABasicinfoMapCtrl();

private:
    ABaiscInfoMap *m_pBaiscInfoMap = nullptr;

};

#endif // ABASICINFOMAPCTRL_H
