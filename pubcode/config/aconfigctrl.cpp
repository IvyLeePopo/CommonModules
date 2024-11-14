#include "../pubcode/config/aconfigctrl.h"
#include <QDebug>

#if defined(COMMONFRAMEMAIN_LIBRARY)
#include "../pubcode/MyEasyLog/aeasylog.h"
#else
#endif

AIniSetting *AConfigCtrl::getConfig()
{
    return m_pConfig;
}

AIniSetting *AConfigCtrl::getLaneInfo()
{
    return m_pLaneInfo;
}

AIniSetting *AConfigCtrl::getXiAnIpAndPort()
{
    return m_pXiAnIpAndPort;
}

void AConfigCtrl::printConfig()
{
    QString sConfigInfo;
    if(m_pConfig)
        sConfigInfo = m_pConfig->getAllConfigInfo();

    //这里是为了处理主程序使用才调这个打印，其它库加载编译不通过的问题
#if defined(COMMONFRAMEMAIN_LIBRARY)
    logInfo(QString("%1 ConfigInfo:\r\n%2").arg(CONFIGFILE).arg(sConfigInfo));
#else
#endif

    QString sLaneInfoConfigInfo;
    if(m_pLaneInfo)
        sLaneInfoConfigInfo = m_pLaneInfo->getAllConfigInfo();

    //这里是为了处理主程序使用才调这个打印，其它库加载编译不通过的问题
#if defined(COMMONFRAMEMAIN_LIBRARY)
    logInfo(QString("%1 ConfigInfo:\r\n%2").arg(LANEINFOFILE).arg(sLaneInfoConfigInfo));
#else
#endif

    QString sXiAnIpAndPortConfigInfo;
    if(m_pXiAnIpAndPort)
        sXiAnIpAndPortConfigInfo = m_pXiAnIpAndPort->getAllConfigInfo();

    //这里是为了处理主程序使用才调这个打印，其它库加载编译不通过的问题
#if defined(COMMONFRAMEMAIN_LIBRARY)
    logInfo(QString("%1 ConfigInfo:\r\n%2").arg(XIANIPANDPORTFILE).arg(sXiAnIpAndPortConfigInfo));
#else
#endif
}

QString AConfigCtrl::getProvinceName()
{
    static QMap<int,QString> qmProvinceCodeAndName =
    {
        {ProvinceCode::BeiJing     ,ProvinceCode::sBeiJing},
        {ProvinceCode::TianJin     ,ProvinceCode::sTianJin},
        {ProvinceCode::HeBei       ,ProvinceCode::sHeBei},
        {ProvinceCode::ShanXi      ,ProvinceCode::sShanXi},
        {ProvinceCode::NeiMengGu   ,ProvinceCode::sNeiMengGu},

        {ProvinceCode::LiaoNing    ,ProvinceCode::sLiaoNing},
        {ProvinceCode::JiLin       ,ProvinceCode::sJiLin},
        {ProvinceCode::HeiLongJiang,ProvinceCode::sHeiLongJiang},

        {ProvinceCode::ShanHai     ,ProvinceCode::sShanHai},
        {ProvinceCode::JiangSu     ,ProvinceCode::sJiangSu},
        {ProvinceCode::ZheJiang    ,ProvinceCode::sZheJiang},
        {ProvinceCode::AnHui       ,ProvinceCode::sAnHui},
        {ProvinceCode::FuJian      ,ProvinceCode::sFuJian},
        {ProvinceCode::JiangXi     ,ProvinceCode::sJiangXi},
        {ProvinceCode::ShanDong    ,ProvinceCode::sShanDong},

        {ProvinceCode::HeNan       ,ProvinceCode::sHeNan},
        {ProvinceCode::HuBei       ,ProvinceCode::sHuBei},
        {ProvinceCode::HuNan       ,ProvinceCode::sHuNan},
        {ProvinceCode::GuangDong   ,ProvinceCode::sGuangDong},
        {ProvinceCode::GuangXi     ,ProvinceCode::sGuangXi},
        {ProvinceCode::HaiNan      ,ProvinceCode::sHaiNan},

        {ProvinceCode::ChongQing   ,ProvinceCode::sChongQing},
        {ProvinceCode::SiChuan     ,ProvinceCode::sSiChuan},
        {ProvinceCode::GuiZhou     ,ProvinceCode::sGuiZhou},
        {ProvinceCode::YunNan      ,ProvinceCode::sYunNan},
        {ProvinceCode::XiZang      ,ProvinceCode::sXiZang},

        {ProvinceCode::Shan3Xi     ,ProvinceCode::sShan3Xi},
        {ProvinceCode::GanSu       ,ProvinceCode::sGanSu},
        {ProvinceCode::QingHai     ,ProvinceCode::sQingHai},
        {ProvinceCode::NingXia     ,ProvinceCode::sNingXia},
        {ProvinceCode::XinJiang    ,ProvinceCode::sXinJiang}
    };

    int nProvinceCode = getProvinceCode();
    if(qmProvinceCodeAndName.contains(nProvinceCode))
        return qmProvinceCodeAndName.value(nProvinceCode);
    else
        return "";
}

int AConfigCtrl::getProvinceCode()
{
    int nProvinceCode = ProvinceCode::TianJin;
    if(m_pConfig)
    {
        QString sLaneId = m_pConfig->getCfgValue("LaneId","laneId","");
        if(false == sLaneId.isEmpty())
            nProvinceCode = sLaneId.left(2).toInt();
        else
            qWarning() << "获取省份编码失败，默认使用天津省份";
    }
    else
        qWarning() << "获取省份编码失败，默认使用天津省份";

    return nProvinceCode;
}

int AConfigCtrl::getLaneType()
{
    int nLaneType = LANETYPE_MTC_ENTRY;
    if(m_pConfig)
    {
        nLaneType = m_pConfig->getCfgValue("LaneType","LaneType",QString::number((int)LANETYPE_MTC_ENTRY)).toInt();
    }
    else
        qWarning() << "获取车道类型失败，默认使用出口混合车道类型";

    return nLaneType;
}

int AConfigCtrl::getRobotModeType()
{
    int nRobotModeType = ERM_INSERT_CARD;
    if(m_pConfig)
    {
        nRobotModeType = m_pConfig->getCfgValue("Mode","RobotMode",QString::number((int)ERM_INSERT_CARD)).toInt();
    }
    else
        qWarning() << "获取机器人类型失败，默认使用插卡式机器人类型";

    return nRobotModeType;
}

bool AConfigCtrl::init()
{
    if(nullptr == m_pConfig)
    {
        m_pConfig = new AIniSetting(CONFIGFILE);
    }

    if(nullptr == m_pLaneInfo)
    {
        m_pLaneInfo = new AIniSetting(LANEINFOFILE);
    }

    if(nullptr == m_pXiAnIpAndPort)
    {
        m_pXiAnIpAndPort = new AIniSetting(XIANIPANDPORTFILE);
    }

    return true;
}

void AConfigCtrl::deInit()
{
    if(m_pConfig)
        delete m_pConfig;
    m_pConfig = nullptr;

    if(m_pLaneInfo)
        delete m_pLaneInfo;
    m_pLaneInfo = nullptr;

    if(m_pXiAnIpAndPort)
        delete m_pXiAnIpAndPort;
    m_pXiAnIpAndPort = nullptr;
}

AConfigCtrl::AConfigCtrl()
{
    init();
}

AConfigCtrl::~AConfigCtrl()
{
    deInit();
}
