/*! ********************************************************
** \brief 通用工具集
**
** 功能详细描述：通用工具集，提供独立的常用的工具函数（静态函数）
** \copyright Copyright  2016，广州华工信息软件有限公司 \n
** All rights reserved
** \file ..\pub\MyTool\apubtool.h/cpp
** \author Ljg
** \date 2016-11-07
**********************************************************/
/*********************************************************
** 修改记录：
** 序号	修改时间	作者 修改内容描述
**
*********************************************************/
//---------------------------------------------------------
#ifndef APUBTOOL_H
#define APUBTOOL_H
//---------------------------------------------------------
#include <QObject>
#include <qglobal.h>
#include <QVariantMap>
#include <QJsonDocument>
//---------------------------------------------------------
/*!
 * \class APubTool
 * \brief 通用工具集类
 *
 * 类功能详细描述:通用工具集，提供独立的常用的工具函数（静态函数）
 */

class APubTool
{
public:
	APubTool();

	static quint64 getDiskFreeSpace(QString sDriver);
	static quint64 getTickCount();
	static bool pingIP(QString sIp);
	static QByteArray variantMapToJSON(const QVariantMap &stData, QJsonDocument::JsonFormat jsonFormat = QJsonDocument::Compact);
	static QByteArray mapToJSON(const QMap<QString,QString> &stData);

	static bool parseJsonToMap(const QByteArray &baJson,QVariantMap &stData);
	static bool parseJsonToMap(const char * pJson,QVariantMap &stData);

	static QByteArray variantListToJSON(const QList<QVariant> &qlData, QJsonDocument::JsonFormat jsonFormat = QJsonDocument::Compact);
	static bool parseJsonToList(const QByteArray &baJson, QList<QVariant> &qlData);

	static QMap<QString, QString> convertMap(const QVariantMap& inputVariantMap);
	static QVariantMap convertMap(const QMap<QString, QString>& inputMap);

	static QString searchFile(const QString& sDirName,const QString& sFileName);

    static QByteArray gbkToUtf8(const QByteArray& gbkByteArray);
    static QString gbkToUtf8(const QString& gbkStr);

    static QByteArray utf8ToGbk(const QByteArray& utf8ByteArray);
    static QString utf8ToGbk(const QString& utf8Str);

	static QString filterChineseChars(const QByteArray &str);
	static bool matchPlate(const QString &s1, const QString &s2, int iNeedMatchCnt);
    static bool matchPlateFilterChineseChars(const QString &s1, const QString &s2, int iNeedMatchCnt);
    static bool matchPlateFilterChineseCharsEx(const QString &s1, const QString &s2, int iNeedMatchCnt);


	static QString unixToLocalTime(const QString &sUnixTimeHex);
	static QString localToUnixTime(const QString &sLocalTimeHex);

	static bool delDir(const QString &path);
	static QDateTime getFileModifyTime(const QString &sFileName);

    static QString getPlateColorText(int iPlateColor);
    static QString getPlateColorText(QString sPlateColor);
    static QString genFullPlateText(QString sPlate, int iPlateColor);
    static QString genFullPlateText(QString sPlate, QString sPlateColor);
    static QString genFullPlateText(QString sVehicleId);

    static unsigned short genCRC16(const unsigned char *crc_data, int len);

    static QByteArray ulong2byte(uint64_t data);
    static QByteArray uint2byte(uint32_t data);
    static QByteArray ushort2byte(uint16_t data);
    static uint64_t byte2ulong(QByteArray data);
    static uint32_t byte2uint(QByteArray data);
    static uint16_t byte2ushort(QByteArray data);
    static void restartPC();
    static void shutdownPC();

    static bool saveToFile(QString sFileName,const QByteArray &baData);
    static QByteArray readFile(QString sFileName);

    static uint16_t crc16MudBus(unsigned char *puchMsg, unsigned short usDataLen);
};
//---------------------------------------------------------
#endif // APUBTOOL_H
