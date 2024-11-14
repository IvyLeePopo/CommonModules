#ifndef IFDATADEF_H
#define IFDATADEF_H
//-------------------------------------------------------------
#include <QMap>
#include <QString>
#include <QVariantMap>
#include <QObject>

#pragma pack(push)
#pragma pack(1)
//-------------------------------------------------------------

//出口云坐席apikey
#define EXIT_EVENT_START                "EXIT_EVENT_START"                  //出口车道特情开始
#define EXIT_EVENT_END                  "EXIT_EVENT_END"                    //出口车道特情结束
#define EXIT_AUTHORIZE                  "EXIT_AUTHORIZE"                    //出口车道业务授权
#define EXIT_AUTHORIZE_CONFIRM          "EXIT_AUTHORIZE_CONFIRM"            //出口车道业务授权确认回复
#define EXIT_CALL_HELP                  "EXIT_CALL_HELP"                    //出口车道发起求助
#define EXIT_UNPAID                     "EXIT_UNPAID"                       //出口车道费额追缴
#define EXIT_UNPAID_CONFIRM             "EXIT_UNPAID_CONFIRM"               //出口车道费额追缴确认回复
#define EXIT_CHECK_ENTRY_INFO           "EXIT_CHECK_ENTRY_INFO"             //出口车道入口信息确认
#define EXIT_CHECK_ENTRY_INFO_CONFIRM   "EXIT_CHECK_ENTRY_INFO_CONFIRM"     //出口车道入口信息确认回复
#define EXIT_CHECK_PLATE_INFO           "EXIT_CHECK_PLATE_INFO"             //出口车道车辆信息确认
#define EXIT_CHECK_PLATE_INFO_CONFIRM   "EXIT_CHECK_PLATE_INFO_CONFIRM"     //出口车道车辆信息确认回复
#define EXIT_CHECK_QUEUE_INFO           "EXIT_CHECK_QUEUE_INFO"             //出口车道队列确认
#define EXIT_CHECK_QUEUE_INFO_CONFIRM   "EXIT_CHECK_QUEUE_INFO_CONFIRM"     //出口车道队列确认回复
#define EXIT_LARGELICENSE               "EXIT_LARGELICENSE"                 //出口大件运输许可证确认
#define EXIT_LARGELICENSE_CONFIRM       "EXIT_LARGELICENSE_CONFIRM"         //出口大件运输许可证确认回复
#define EXIT_NOTIFY_TYPE                "EXIT_NOTIFY_TYPE"                  //刷卡状态
#define EXIT_PAY_RESULT                 "EXIT_PAY_RESULT"                   //支付结果
#define EXIT_SHOW_FEE                   "EXIT_SHOW_FEE"                     //出口费显
#define EXIT_UP_CMD                     "EXIT_UP_CMD"                       //出口云坐席主动发起指令
#define EXIT_UP_CMD_CONFIRM             "EXIT_UP_CMD_CONFIRM"               //出口车道回复云坐席主动指令执行结果
#define EXIT_HAND_CAPTURE               "EXIT_HAND_CAPTURE"                 //出口车道手动抓拍信息上报
#define EXIT_ROBOT_TAKE_OVER            "EXIT_ROBOT_TAKE_OVER"              //出口车道机器人工单接管

//入口云坐席apikey
#define ENTRY_EVENT_START               "ENTRY_EVENT_START"                 //入口车道特情开始
#define ENTRY_EVENT_END                 "ENTRY_EVENT_END"                   //入口车道特情结束
#define ENTRY_AUTHORIZE                 "ENTRY_AUTHORIZE"                   //入口车道业务授权
#define ENTRY_AUTHORIZE_CONFIRM         "ENTRY_AUTHORIZE_CONFIRM"           //入口车道业务授权确认回复
#define ENTRY_TAKE_CARD                 "ENTRY_TAKE_CARD"                   //入口车道取卡状态
#define ENTRY_CHECK_WEIGHT_INFO         "ENTRY_CHECK_WEIGHT_INFO"           //入口车道称重信息确认
#define ENTRY_CHECK_WEIGHT_INFO_CONFIRM "ENTRY_CHECK_WEIGHT_INFO_CONFIRM"   //入口车道称重信息确认回复
#define ENTRY_CALL_HELP                 "ENTRY_CALL_HELP"                   //入口车道发起求助
#define ENTRY_UNPAID                    "ENTRY_UNPAID"                      //入口车道费额追缴
#define ENTRY_UNPAID_CONFIRM            "ENTRY_UNPAID_CONFIRM"              //入口车道费额追缴确认回复
#define ENTRY_CHECK_PLATE_INFO          "ENTRY_CHECK_PLATE_INFO"            //入口车道车辆信息确认
#define ENTRY_CHECK_PLATE_INFO_CONFIRM  "ENTRY_CHECK_PLATE_INFO_CONFIRM"    //入口车道车辆信息确认回复
#define ENTRY_CHECK_QUEUE_INFO          "ENTRY_CHECK_QUEUE_INFO"            //入口车道队列确认
#define ENTRY_CHECK_QUEUE_INFO_CONFIRM  "ENTRY_CHECK_QUEUE_INFO_CONFIRM"    //入口车道队列确认回复
#define ENTRY_LARGELICENSE              "ENTRY_LARGELICENSE"                //入口大件运输许可证确认
#define ENTRY_LARGELICENSE_CONFIRM      "ENTRY_LARGELICENSE_CONFIRM"        //入口大件运输许可证确认回复
#define ENTRY_UP_CMD                    "ENTRY_UP_CMD"                      //入口云坐席主动发起指令
#define ENTRY_UP_CMD_CONFIRM            "ENTRY_UP_CMD_CONFIRM"              //入口车道回复云坐席主动指令执行结果
#define ENTRY_HAND_CAPTURE              "ENTRY_HAND_CAPTURE"                //入口车道手动抓拍信息上报
#define ENTRY_ROBOT_TAKE_OVER           "ENTRY_ROBOT_TAKE_OVER"             //入口车道机器人工单接管

//出入口云坐席共用apikey
#define UP_MESSAGE                      "UP_MESSAGE"                        //车道消息通知
#define TRIGGER_ALARM                   "TRIGGER_ALARM"                     //机器人报警

/*!
 * \brief 通行券异常状态
 */
enum
{
   Bad_Card   = 1,     //坏卡
   Lost_Card  = 2      //丢卡
};

enum EMediaType
{
    EMT_Init        = 0,    //初始值
    EMT_OBU         = 1,    //OBU(ETC)
    EMT_CPC         = 2,    //CPC
    EMT_Paper       = 3,    //纸券
    EMT_Null        = 9,    //无通行介质
    EMT_SignalETC   = 10,   //ETC单卡

};

/*!
 * \brief 车型分类定义
 */
enum EVehType
{
    VehType_Guest_1 = 1,           /*!< 客一 */
    VehType_Guest_2 = 2,           /*!< 客二 */
    VehType_Guest_3 = 3,           /*!< 客三 */
    VehType_Guest_4 = 4,           /*!< 客四 */

    VehType_Goods_1 = 11,           /*!< 货一 */
    VehType_Goods_2 = 12,           /*!< 货二 */
    VehType_Goods_3 = 13,           /*!< 货三 */
    VehType_Goods_4 = 14,           /*!< 货四 */
    VehType_Goods_5 = 15,           /*!< 货五 */
    VehType_Goods_6 = 16,           /*!< 货六 */

    VehType_SpecialOperation_1 = 21,/*!< 专一 */
    VehType_SpecialOperation_2 = 22,/*!< 专二 */
    VehType_SpecialOperation_3 = 23,/*!< 专三 */
    VehType_SpecialOperation_4 = 24,/*!< 专四 */
    VehType_SpecialOperation_5 = 25,/*!< 专五 */
    VehType_SpecialOperation_6 = 26,/*!< 专六 */
};

/*!
 * \brief 国标车种定义
 */
enum EGBVehClass
{
    GB_VEHC_NORMAL = 0,					/*!< 正常车 */
//		GB_VEHC_OFFICE = 6,					/*!< 公务车 */
    GB_VEHC_ARMY = 8,					/*!< 军警车 */
    GB_VEHC_URGENCY = 10,				/*!< 紧急车 */
//		GB_VEHC_FREE = 12,					/*!< 免费车 */
    GB_VEHC_CARTEAM = 14,				/*!< 车队车 */
    GB_VEHC_GREENCAR = 21,				/*!< 绿通车 */
    GB_VEHC_COMBINE = 22,				/*!< 联合收割机 */
    GB_VEHC_RESCUE = 23,				/*!< 抢险救灾 */
    GB_VEHC_CONTAINER = 24,				/*!< 集装箱 */
    GB_VEHC_LARGETRANSPORTATION = 25,	/*!< 大件运输 */
    GB_VEHC_EMERGENCY = 26,				/*!< 应急车 */
    GB_VEHC_TRANORSEMITRAILER = 27,     /*!< 货车列车或半挂汽车列车 */
    GB_VEHC_J2_CONTAINER = 28,			/*!< J2集装箱 */
};

enum EGBVehPlateColor
{
    GB_VEHC_BLUE            = 0,        //蓝色
    GB_VEHC_YELLOW          = 1,        //黄色
    GB_VEHC_BLACK           = 2,        //黑色
    GB_VEHC_WHITE           = 3,        //白色
    GB_VEHC_LIGHT_GREEN     = 4,        //渐绿
    GB_VEHC_YELLOWANDGREEN  = 5,        //黄绿
    GB_VEHC_BLUEANDWHITE    = 6,        //蓝白
    GB_VEHC_TMP             = 7,        //临时
    GB_VEHC_GREEN           = 11,       //绿色
    GB_VEHC_RED             = 12,       //红色

};

//车道类型定义
enum
{
    LANETYPE_MTC_ENTRY=1,			//MTC入口道
    LANETYPE_MTC_EXPORT=2,			//MTC出口道
    LANETYPE_ETC_ENTRY=3,			//ETC入口道
    LANETYPE_ETC_EXPORT=4,			//ETC出口道
    LANETYPE_MTC_OPEN=5,			//MTC开放道
    LANETYPE_ETC_OPEN=6, 			//ETC开放道
    LANETYPE_AUTO_SENDCARD=16,      //自助发卡道
};

//机器人类型定义
enum
{
    ERM_INSERT_CARD  = 0,           //插卡式机器人
    ERM_THROWING_CARD_GD = 1,       //抛卡机器人（广电）
};

//控制设备指令
enum EDeviceControl{
    CTL_REBOOT_NODE_B = 2,          //重启B节点指令
    CTL_RAILING_UP,                 //抬杆指令
    CTL_RAILING_DOWN,               //降杆指令
    CTL_FORWARD_IN,                 //开启关道机（伸出）
    CTL_FORWARD_OUT,                //关闭关道机（缩回）
    CTL_CANOPY_ON,                  //开启雨棚灯
    CTL_CANOPY_OFF,                 //关闭雨棚灯

    CTL_CHANGE_BAK = 11,            //切换到灾备环境
    CTL_CHANGE_NORM,                //切换到正常环境

    CTL_REBOOT_GATEWAY = 51,        //重启智能网关

    CTL_HAND_RSU_OPEN = 100,        //开启手持天线
    CTL_HAND_RSU_CLOSE = 101,       //关闭手持天线
};

#pragma pack(pop)
//-------------------------------------------------------------
#endif // IFDATADEF_H

