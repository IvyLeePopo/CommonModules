#ifndef ABAISCINFOMAP_H
#define ABAISCINFOMAP_H

#include <QObject>
#include <QMutex>
#include <QMap>

namespace NS_BaiscInfoMap{
    const QString sStationId            = "stationId";
    const QString sStationName          = "stationName";
    const QString sStationHex           = "stationHex";

    const QString sStationNamePinYing   = "stationNamePinYing";
    const QString sStationNameJianPin   = "stationNameJianPin";

}

class ABaiscInfoMap: QObject
{
    Q_OBJECT
public:
    explicit ABaiscInfoMap(QObject *parent = Q_NULLPTR);
    virtual ~ABaiscInfoMap();

    QString queryStationNameByStationHex(const QString sStationHex);
    QString queryStationIdByStationHex(const QString sStationHex);
    bool queryStationNameByPartName(const QString sPartName, QList<QVariantMap> &qlVmap);
    bool queryStationNameByPinYing(const QString sPinYing, QList<QVariantMap> &qlVmap, int nProvinceCode);
    bool queryStationInfoByProvinceCode(QList<QVariantMap> &qlVmap, int nProvinceCode);

    bool updateBasicinfoMap(const QByteArray baParams,bool bOnlyUpdateCache = false);

signals:
    void sigLoadBaiscInfoMapParam();
    void sigUpdateBaiscInfoMapParam();

protected slots:
    void sltLoadBaiscInfoMapParam();
    void sltUpdateBaiscInfoMapParam();

protected:
    QMap<QString,QVariantMap> m_mapStationHexAndBasicInfo; //存放收费站Hex编码和收费站信息
    QMap<QString,QVariantMap> m_mapStationIdAndBasicInfo; //存放收费站Id和收费站信息

    QMutex m_mutex;
    bool m_bIsFirstLoad;

};

#endif // ABAISCINFOMAP_H
