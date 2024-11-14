#ifndef COMMONFUNCTIONS_H
#define COMMONFUNCTIONS_H

#include <iostream>
#include <QString>
#include <QPair>
namespace NS_CommonFunc
{
	extern bool isVehTypeValid(int nVehType);
    extern bool isPlateColorValid(int nPlateColor);
	extern bool isPlateValid(int nPlateColor, const QString& sPlate);
	extern bool isVehicleIdValid(const QString& sVehicleID);
	extern QMap<int,QString> getPlateColorTextMap();
	extern QMap<int,QString> getLongPlateColorTextMap();
    extern QString getVehColorNameByColor(int nColor);
    extern int getVehColorByColorName(QString sVehColorName);

	extern QString getPlateWithColor(int nPlateColor,const QString& sPlate);
	extern QPair<int,QString> splitPlateAndColor(const QString& sPlateWithColor);
    extern QString getVehTypeName(int nVehType);
    extern QString getVehClassName(int nVehClass);
    extern int getVehAxleCntByVehType(int nVehType);

    extern QString getMediaTypeName(int nMediaType);
    extern QString getFeeCalcTypeName(int nCalcType);


	extern bool isMatchGangAoPlate(const QString& sPlate);

    extern void GetVehTypeAndClass(QString& Type, QString& Class, int VehType, int VehClass = 0);

    extern QMap<QString, QString> getProvinceNameAndProvinceJP();
    extern QMap<QString, QString> getProvinceNameAndProvincePY();
    extern QMap<QString, int> getProvinceNameAndProvinceCode();
    extern QString getProvinceName(QString sLaneHex);

    extern QString getExitBusinessAuthReasonHuNan(QString sBussinessAuthorizType);
    extern QString getExitBusinessAuthReason(QString sBussinessAuthorizType);
    extern QString getEntryBusinessAuthReason(QString sBussinessAuthorizType);

    extern void delayMs(int milliseconds);

}
#endif //COMMONFUNCTIONS_H
