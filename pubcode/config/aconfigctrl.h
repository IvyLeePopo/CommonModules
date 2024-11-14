#ifndef ACONFIGCTRL_H
#define ACONFIGCTRL_H

#include "../pubcode/pub/Singleton/asingleton.h"
#include "../pubcode/pub/MySetting/ainisetting.h"
#include "../pubcode/IFDataDef/KeyNameDef/aprovincecode.h"
#include "../pubcode/IFDataDef/ifdatadef.h"

#ifdef Q_OS_WIN32
    #define CONFIGFILE QString::fromUtf8("D:/Robot/Config.ini")
    #define LANEINFOFILE QString::fromUtf8("D:/Robot/LaneInfo.ini")
    #define XIANIPANDPORTFILE QString::fromUtf8("D:/Robot/XiAnIpAndPort.ini")
#else
    #define CONFIGFILE QString::fromUtf8("/opt/scaner/lib/Config.ini")
    #define LANEINFOFILE QString::fromUtf8("/opt/scaner/lib/LaneInfo.ini")
    #define XIANIPANDPORTFILE QString::fromUtf8("/opt/scaner/lib/XiAnIpAndPort.ini")
#endif

#define CONFIG AConfigCtrl::getInstance().getConfig()
#define LANEINFOCONFIG AConfigCtrl::getInstance().getLaneInfo()
#define XIANIPANDPORTCONFIG AConfigCtrl::getInstance().getXiAnIpAndPort()

class AConfigCtrl: public ASingleton<AConfigCtrl>
{
    friend class ASingleton<AConfigCtrl>;

public:
    virtual ~AConfigCtrl();

    AIniSetting *getConfig();
    AIniSetting *getLaneInfo();
    AIniSetting *getXiAnIpAndPort();
    void printConfig();

    //获取省份名称
    QString getProvinceName();
    //获取省份编码
    int getProvinceCode();

    //获取车道类型
    int getLaneType();

    //获取机器人类型
    int getRobotModeType();



private:
    explicit AConfigCtrl();

    bool init();
    void deInit();

private:
    AIniSetting *m_pConfig = nullptr;
    AIniSetting *m_pLaneInfo = nullptr;
    AIniSetting *m_pXiAnIpAndPort = nullptr;

};

#endif // ACONFIGCTRL_H
