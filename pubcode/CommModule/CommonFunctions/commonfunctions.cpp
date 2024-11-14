#include "commonfunctions.h"
#include <QMap>
#include <QSet>
#include <QEventLoop>
#include <QTimer>
#include "../../IFDataDef/KeyNameDef/aprovincecode.h"

bool NS_CommonFunc::isVehTypeValid(int nVehType)
{
	if((1 <= nVehType && nVehType <= 4)
		|| (11 <= nVehType && nVehType <= 16)
		|| (21 <= nVehType && nVehType <= 26)
	  )
	{
		return true;
	}
	return false;
}

bool NS_CommonFunc::isPlateColorValid(int nPlateColor)
{
	QSet<int> sValidPlateColor{0,1,2,3,4,5,6,7,9,11,12};
	return sValidPlateColor.contains(nPlateColor);
}

QMap<int,QString> NS_CommonFunc::getPlateColorTextMap()
{
	static QMap<int,QString> qmColorText =
	{
        {0,QString::fromUtf8("蓝")},
        {1,QString::fromUtf8("黄")},
        {2,QString::fromUtf8("黑")},
        {3,QString::fromUtf8("白")},
        {4,QString::fromUtf8("渐绿")},
        {5,QString::fromUtf8("黄绿")},
        {6,QString::fromUtf8("蓝白")},
        {7,QString::fromUtf8("临时")},
        {9,QString::fromUtf8("")},
        {11,QString::fromUtf8("绿")},
        {12,QString::fromUtf8("红")}
	};
	return qmColorText;
}

QMap<int,QString> NS_CommonFunc::getLongPlateColorTextMap()
{
	static QMap<int,QString> qmColorText =
	{
        {0,QString::fromUtf8("蓝色")},
        {1,QString::fromUtf8("黄色")},
        {2,QString::fromUtf8("黑色")},
        {3,QString::fromUtf8("白色")},
        {4,QString::fromUtf8("渐绿")},
        {5,QString::fromUtf8("黄绿")},
        {6,QString::fromUtf8("蓝白")},
        {7,QString::fromUtf8("临时")},
        {9,QString::fromUtf8("")},
        {11,QString::fromUtf8("绿色")},
        {12,QString::fromUtf8("红色")}
	};
	return qmColorText;
}

QPair<int, QString> NS_CommonFunc::splitPlateAndColor(const QString &sPlateWithColor)
{
	int nPlateColor = 9;
	int nColorLen = 0;
	QString sPlate = "";
	QMap<int,QString> qmColorText = getPlateColorTextMap();
	for(int color:qmColorText.keys())
	{
		if(sPlateWithColor.startsWith(qmColorText[color])
			&& qmColorText[color].length() > nColorLen)
		{
			nColorLen = qmColorText[color].length();
			nPlateColor = color;
		}
	}
	static QMap<QString,int> qmExtraColorDesc =
	{
        {QString::fromUtf8("拼"),5},
        {QString::fromUtf8("渐"),6},
        {QString::fromUtf8("临"),7},
        {QString::fromUtf8("未"),9},
        {QString::fromUtf8("未知"),9}
	};
	if(0 == nColorLen)
	{
		for(QString sColor:qmExtraColorDesc.keys())
		{
			if(sPlateWithColor.startsWith(sColor))
			{
				nColorLen = sColor.length();
				nPlateColor = qmExtraColorDesc[sColor];
			}
		}
	}
	sPlate = sPlateWithColor.mid(nColorLen);
	return QPair<int,QString>(nPlateColor,sPlate);
}

QString NS_CommonFunc::getPlateWithColor(int nPlateColor, const QString &sPlate)
{
	QMap<int,QString> qmColorText = getPlateColorTextMap();
	QString sPlateColorText = qmColorText.value(nPlateColor);
	return sPlateColorText + sPlate;
}

QString NS_CommonFunc::getVehTypeName(int nVehType)
{
    static QMap<int,QString> qmVehTypeText =
    {
        {0,QString::fromUtf8("客一")},
        {1,QString::fromUtf8("客一")},
        {2,QString::fromUtf8("客二")},
        {3,QString::fromUtf8("客三")},
        {4,QString::fromUtf8("客四")},

        {11,QString::fromUtf8("货一")},
        {12,QString::fromUtf8("货二")},
        {13,QString::fromUtf8("货三")},
        {14,QString::fromUtf8("货四")},
        {15,QString::fromUtf8("货五")},
        {16,QString::fromUtf8("货六")},

        {21,QString::fromUtf8("专一")},
        {22,QString::fromUtf8("专二")},
        {23,QString::fromUtf8("专三")},
        {24,QString::fromUtf8("专四")},
        {25,QString::fromUtf8("专五")},
        {26,QString::fromUtf8("专六")}
    };

    if(qmVehTypeText.contains(nVehType))
        return qmVehTypeText.value(nVehType);
    else
        return QString::fromUtf8("未知车型");
}

bool NS_CommonFunc::isPlateValid(int nPlateColor,const QString &sPlate)
{
    QString sVehicleID = sPlate.trimmed() + QString::fromUtf8("_") + QString::number(nPlateColor);
	return isVehicleIdValid(sVehicleID);
}

bool NS_CommonFunc::isVehicleIdValid(const QString &sVehicleID)
{
    QRegExp rx(QString::fromUtf8("^[京津沪渝冀豫云辽黑湘皖鲁新苏浙赣鄂桂甘晋蒙陕吉闽贵粤青藏川宁琼使领A-Z挂学警港澳领试超外0-9]{2,9}_(0|1|2|3|4|5|6|7|11|12)$|^[京津沪渝冀豫云辽黑湘皖鲁新苏浙赣鄂桂甘晋蒙陕吉闽贵粤青藏川宁琼][A-Z0-9][A-Z0-9]{4}应急_(0|1|2|3|4|5|6|7|11|12)$|^默A00000_[9]$"));
	if(rx.indexIn(sVehicleID) >= 0)
	{
		return true;
	}
	return false;
}

bool NS_CommonFunc::isMatchGangAoPlate(const QString &sPlate)
{
	if(sPlate.toLocal8Bit().size() >= 8)
	{
		return false;
	}
	if(false == isPlateValid(0,sPlate))
	{
		return false;
	}
    QRegExp rx(QString::fromUtf8("^[A-Z][^京津沪渝冀豫云辽黑湘皖鲁新苏浙赣鄂桂甘晋蒙陕吉闽贵粤青藏川宁]{1,7}$"));
	if(rx.indexIn(sPlate) >= 0)
	{
		return true;
	}
	return false;
}

void NS_CommonFunc::GetVehTypeAndClass(QString &Type, QString &Class, int VehType, int VehClass)
{
    switch(VehType)
    {
    case 1:
        Type  = "客一";
        break;
    case 2:
        Type  = "客二";
        break;
    case 3:
        Type  = "客三";
        break;
    case 4:
        Type  = "客四";
        break;
    case 11:
        Type  = "货一";
        break;
    case 12:
        Type  = "货二";
        break;
    case 13:
        Type  = "货三";
        break;
    case 14:
        Type  = "货四";
        break;
    case 15:
        Type  = "货五";
        break;
    case 16:
        Type  = "货六";
        break;
    case 21:
        Type  = "专一";
        break;
    case 22:
        Type  = "专二";
        break;
    case 23:
        Type  = "专三";
        break;
    case 24:
        Type  = "专四";
        break;
    case 25:
        Type  = "专五";
        break;
    case 26:
        Type  = "专六";
        break;
    default:
        Type  = "客一";
        break;
    }
    switch(VehClass)
    {
    case 0:
        Class = "普通车";
        break;
    case 6:
        Class = "公务车";
        break;
    case 8:
        Class = "军警车";
        break;
    case 10:
        Class = "紧急";
        break;
    case 14:
        Class = "车队";
        break;
    case 21:
        Class = "绿通车";
        break;
    case 22:
        Class = "联合收割机";
        break;
    case 23:
        Class = "抢险救灾";
        break;
    case 24:
        Class = "集装箱";
        break;
    case 25:
        Class = "大件运输";
        break;
    case 26:
        Class = "应急车";
        break;
    case 27:
        Class = "货车列车";
        break;
    case 28:
        Class = "集装箱";
        break;
    default:
        Class = "普通车";
        break;
    }

    return;
}

QString NS_CommonFunc::getVehColorNameByColor(int nColor)
{
    static QMap<int,QString> qmColorAndText =
    {
        {0,QString::fromUtf8("蓝色")},
        {1,QString::fromUtf8("黄色")},
        {2,QString::fromUtf8("黑色")},
        {3,QString::fromUtf8("白色")},
        {4,QString::fromUtf8("渐绿")},
        {5,QString::fromUtf8("黄绿")},
        {6,QString::fromUtf8("蓝白")},
        {7,QString::fromUtf8("临时")},
        {9,QString::fromUtf8("")},
        {11,QString::fromUtf8("绿色")},
        {12,QString::fromUtf8("红色")}
    };

    if(qmColorAndText.contains(nColor))
        return qmColorAndText.value(nColor);
    else
        return QString::fromUtf8("");
}

int NS_CommonFunc::getVehColorByColorName(QString sVehColorName)
{
    static QMap<QString,int> qmTextAndColorText =
    {
        {QString::fromUtf8("蓝色"),0},
        {QString::fromUtf8("黄色"),1},
        {QString::fromUtf8("黑色"),2},
        {QString::fromUtf8("白色"),3},
        {QString::fromUtf8("渐绿"),4},
        {QString::fromUtf8("黄绿"),5},
        {QString::fromUtf8("蓝白"),6},
        {QString::fromUtf8("临时"),7},
        {QString::fromUtf8(""),9},
        {QString::fromUtf8("绿色"),11},
        {QString::fromUtf8("红色"),12}
    };

    if(qmTextAndColorText.contains(sVehColorName))
        return qmTextAndColorText.value(sVehColorName);
    else
        return 9;
}

QString NS_CommonFunc::getVehClassName(int nVehClass)
{
    static QMap<int,QString> qmVehClassText =
    {
        {0,QString::fromUtf8("普通车")},
        {8,QString::fromUtf8("军警车")},
        {10,QString::fromUtf8("紧急车")},
        {14,QString::fromUtf8("车队车")},
        {21,QString::fromUtf8("绿通车")},
        {22,QString::fromUtf8("联合收割机")},
        {23,QString::fromUtf8("抢险救灾车")},
        {24,QString::fromUtf8("J1集装箱")},
        {25,QString::fromUtf8("大件运输")},
        {26,QString::fromUtf8("应急车")},
        {27,QString::fromUtf8("牵引车")},
        {28,QString::fromUtf8("J2集装箱")}
    };

    if(qmVehClassText.contains(nVehClass))
        return qmVehClassText.value(nVehClass);
    else
        return QString::fromUtf8("普通车");
}

int NS_CommonFunc::getVehAxleCntByVehType(int nVehType)
{
    int nAxleCnt = 2;
    if(10 < nVehType)
    {
        nAxleCnt = nVehType%10;
        if(2 >= nAxleCnt)
            nAxleCnt = 2;
    }

    return nAxleCnt;
}

QString NS_CommonFunc::getMediaTypeName(int nMediaType)
{
    static QMap<int,QString> qmMediaTypeText =
    {
        {1,QString::fromUtf8("OBU")},
        {2,QString::fromUtf8("CPC")},
        {3,QString::fromUtf8("纸券")},
        {9,QString::fromUtf8("无通行介质")}
    };

    if(qmMediaTypeText.contains(nMediaType))
        return qmMediaTypeText.value(nMediaType);
    else
        return QString::fromUtf8("无通行介质");
}

QMap<QString, QString> NS_CommonFunc::getProvinceNameAndProvinceJP()
{
    static QMap<QString,QString> mapProvinceNameAndProvinceJP =
    {
        {ProvinceCode::sBeiJingName,       "bj"},
        {ProvinceCode::sTianJinName,       "tj"},
        {ProvinceCode::sHeBeiName,         "hb"},
        {ProvinceCode::sShanXiName,        "sx"},
        {ProvinceCode::sNeiMengGuName,     "nmg"},
        {ProvinceCode::sLiaoNingName,      "ln"},
        {ProvinceCode::sJiLinName,         "jl"},
        {ProvinceCode::sHeiLongJiangName,  "hlj"},
        {ProvinceCode::sShanHaiName,       "sh"},
        {ProvinceCode::sJiangSuName,       "js"},
        {ProvinceCode::sZheJiangName,      "zj"},
        {ProvinceCode::sAnHuiName,         "ah"},
        {ProvinceCode::sFuJianName,        "fj"},
        {ProvinceCode::sJiangXiName,       "jx"},
        {ProvinceCode::sShanDongName,      "sd"},
        {ProvinceCode::sHeNanName,         "hn"},
        {ProvinceCode::sHuBeiName,         "hb"},
        {ProvinceCode::sHuNanName,         "hn"},
        {ProvinceCode::sGuangDongName,     "gd"},
        {ProvinceCode::sGuangXiName,       "gx"},
        {ProvinceCode::sHaiNanName,        "hn"},
        {ProvinceCode::sChongQingName,     "cq"},
        {ProvinceCode::sSiChuanName,       "sc"},
        {ProvinceCode::sGuiZhouName,       "gz"},
        {ProvinceCode::sYunNanName,        "yn"},
        {ProvinceCode::sXiZangName,        "xz"},
        {ProvinceCode::sShan3XiName,       "sx"},
        {ProvinceCode::sGanSuName,         "gs"},
        {ProvinceCode::sQingHaiName,       "qh"},
        {ProvinceCode::sNingXiaName,       "nx"},
        {ProvinceCode::sXinJiangName,      "xj"}
    };
    return mapProvinceNameAndProvinceJP;
}

QMap<QString, QString> NS_CommonFunc::getProvinceNameAndProvincePY()
{
    static QMap<QString,QString> mapProvinceNameAndProvincePY =
    {
        {ProvinceCode::sBeiJingName,       "beijing"},
        {ProvinceCode::sTianJinName,       "tianjin"},
        {ProvinceCode::sHeBeiName,         "hebei"},
        {ProvinceCode::sShanXiName,        "shanxi"},
        {ProvinceCode::sNeiMengGuName,     "neimenggu"},
        {ProvinceCode::sLiaoNingName,      "liaoning"},
        {ProvinceCode::sJiLinName,         "jilin"},
        {ProvinceCode::sHeiLongJiangName,  "heilongjiang"},
        {ProvinceCode::sShanHaiName,       "shanhai"},
        {ProvinceCode::sJiangSuName,       "jiangsu"},
        {ProvinceCode::sZheJiangName,      "zhejiang"},
        {ProvinceCode::sAnHuiName,         "anhui"},
        {ProvinceCode::sFuJianName,        "fujian"},
        {ProvinceCode::sJiangXiName,       "jiangxi"},
        {ProvinceCode::sShanDongName,      "shandong"},
        {ProvinceCode::sHeNanName,         "henan"},
        {ProvinceCode::sHuBeiName,         "hubei"},
        {ProvinceCode::sHuNanName,         "hunan"},
        {ProvinceCode::sGuangDongName,     "guangdong"},
        {ProvinceCode::sGuangXiName,       "guangxi"},
        {ProvinceCode::sHaiNanName,        "hainan"},
        {ProvinceCode::sChongQingName,     "chongqing"},
        {ProvinceCode::sSiChuanName,       "sichuan"},
        {ProvinceCode::sGuiZhouName,       "guizhou"},
        {ProvinceCode::sYunNanName,        "yunnan"},
        {ProvinceCode::sXiZangName,        "xizang"},
        {ProvinceCode::sShan3XiName,       "shanxi"},
        {ProvinceCode::sGanSuName,         "gansu"},
        {ProvinceCode::sQingHaiName,       "qinghai"},
        {ProvinceCode::sNingXiaName,       "ningxia"},
        {ProvinceCode::sXinJiangName,      "xinjiang"}
    };
    return mapProvinceNameAndProvincePY;
}

QMap<QString, int> NS_CommonFunc::getProvinceNameAndProvinceCode()
{
    static QMap<QString, int> mapProvinceNameAndProvinceCode =
    {
        {ProvinceCode::sBeiJingName,     ProvinceCode::BeiJing},
        {ProvinceCode::sTianJinName,     ProvinceCode::TianJin},
        {ProvinceCode::sHeBeiName,       ProvinceCode::HeBei},
        {ProvinceCode::sShanXiName,      ProvinceCode::ShanXi},
        {ProvinceCode::sNeiMengGuName,   ProvinceCode::NeiMengGu},
        {ProvinceCode::sLiaoNingName,    ProvinceCode::LiaoNing},
        {ProvinceCode::sJiLinName,       ProvinceCode::JiLin},
        {ProvinceCode::sHeiLongJiangName,ProvinceCode::HeiLongJiang},
        {ProvinceCode::sShanHaiName,     ProvinceCode::ShanHai},
        {ProvinceCode::sJiangSuName,     ProvinceCode::JiangSu},
        {ProvinceCode::sZheJiangName,    ProvinceCode::ZheJiang},
        {ProvinceCode::sAnHuiName,       ProvinceCode::AnHui},
        {ProvinceCode::sFuJianName,      ProvinceCode::FuJian},
        {ProvinceCode::sJiangXiName,     ProvinceCode::JiangXi},
        {ProvinceCode::sShanDongName,    ProvinceCode::ShanDong},
        {ProvinceCode::sHeNanName,       ProvinceCode::HeNan},
        {ProvinceCode::sHuBeiName,       ProvinceCode::HuBei},
        {ProvinceCode::sHuNanName,       ProvinceCode::HuNan},
        {ProvinceCode::sGuangDongName,   ProvinceCode::GuangDong},
        {ProvinceCode::sGuangXiName,     ProvinceCode::GuangXi},
        {ProvinceCode::sHaiNanName,      ProvinceCode::HaiNan},
        {ProvinceCode::sChongQingName,   ProvinceCode::ChongQing},
        {ProvinceCode::sSiChuanName,     ProvinceCode::SiChuan},
        {ProvinceCode::sGuiZhouName,     ProvinceCode::GuiZhou},
        {ProvinceCode::sYunNanName,      ProvinceCode::YunNan},
        {ProvinceCode::sXiZangName,      ProvinceCode::XiZang},
        {ProvinceCode::sShan3XiName,     ProvinceCode::Shan3Xi},
        {ProvinceCode::sGanSuName,       ProvinceCode::GanSu},
        {ProvinceCode::sQingHaiName,     ProvinceCode::QingHai},
        {ProvinceCode::sNingXiaName,     ProvinceCode::NingXia},
        {ProvinceCode::sXinJiangName,    ProvinceCode::XinJiang},
    };
    return mapProvinceNameAndProvinceCode;
}

QString NS_CommonFunc::getProvinceName(QString sLaneHex)
{
    if(sLaneHex.isEmpty())
        return "";

    QMap<QString, int> mapProvinceNameAndProvinceCode = getProvinceNameAndProvinceCode();
    QMap<QString, int>::iterator it = mapProvinceNameAndProvinceCode.begin();
    for(;it != mapProvinceNameAndProvinceCode.end();it++)
    {
        int nCode = it.value();
        if(sLaneHex.startsWith(QString::number(nCode)))
            return it.key();
    }
    return "";
}

QString NS_CommonFunc::getExitBusinessAuthReason(QString sBussinessAuthorizType)
{
    static QMap<QString,QString> qmBusinessAuthText =
    {
        {"001",QString::fromUtf8("货车超载")},
        {"002",QString::fromUtf8("节假日放行确认")},
        {"003",QString::fromUtf8("绿通查验")},
        {"004",QString::fromUtf8("联合收割机查验")},

        {"008",QString::fromUtf8("黑名单车辆")},
        {"009",QString::fromUtf8("最小费额授权")},
        {"010",QString::fromUtf8("回写CPC卡信息")},
        {"011",QString::fromUtf8("本省警车授权")},
        {"012",QString::fromUtf8("抛卡机器人存在已交易通行卡授权")},
        {"013",QString::fromUtf8("超时车异常拦截")},
        {"014",QString::fromUtf8("省内车牌黑名单")},
        {"015",QString::fromUtf8("U转车查验")},
        {"016",QString::fromUtf8("出口治超")},
        {"017",QString::fromUtf8("重放行授权")},
        {"018",QString::fromUtf8("天线交易重放行授权")},
        {"019",QString::fromUtf8("收到插CPC通知是否转CPC交易确认")},
        {"020",QString::fromUtf8("U转车业务授权")}
    };

    if(qmBusinessAuthText.contains(sBussinessAuthorizType))
        return qmBusinessAuthText.value(sBussinessAuthorizType);
    else
        return QString::fromUtf8("未知业务授权");
}

QString NS_CommonFunc::getEntryBusinessAuthReason(QString sBussinessAuthorizType)
{
    static QMap<QString,QString> qmBusinessAuthText =
    {
        {"001",QString::fromUtf8("云坐席授权发卡")},
        {"002",QString::fromUtf8("两客一危授权通行")},
        {"003",QString::fromUtf8("货车无治超数据")},
        {"004",QString::fromUtf8("治超数据异常")},
        {"005",QString::fromUtf8("货车超载")},
        {"006",QString::fromUtf8("已交易放行确认")},
        {"007",QString::fromUtf8("节假日放行确认")},
        {"008",QString::fromUtf8("黑名单车辆确认")},
        {"009",QString::fromUtf8("入口大件运输查验")},
        {"010",QString::fromUtf8("入口治超确认")},
        {"011",QString::fromUtf8("危险品车确认")},
        {"012",QString::fromUtf8("牵引车授权")},
        {"013",QString::fromUtf8("专项作业车入口超限超载放行")},
        {"014",QString::fromUtf8("限行通行车")},
        {"015",QString::fromUtf8("车辆运输车")},
        {"016",QString::fromUtf8("入口大件运输ETC查验")},
        {"017",QString::fromUtf8("长宽高超限")},
        {"018",QString::fromUtf8("入口大件运输")},
        {"019",QString::fromUtf8("长宽高超限ETC查验")},
        {"020",QString::fromUtf8("etc车型车牌校验")},
        {"021",QString::fromUtf8("长宽高超限")},
        {"022",QString::fromUtf8("车型与轴数不一致")},
        {"023",QString::fromUtf8("车牌重放行ETC")},
        {"024",QString::fromUtf8("车牌重放行CPC")},
        {"025",QString::fromUtf8("治超数据异常ETC")},
        {"026",QString::fromUtf8("黑名单车辆确认ETC")},
        {"027",QString::fromUtf8("长宽高超限ETC")},
        {"028",QString::fromUtf8("省内黑名单车辆确认CPC")},
        {"029",QString::fromUtf8("交易确认失败")},
        {"030",QString::fromUtf8("专项作业车查验ETC")},
        {"031",QString::fromUtf8("专项作业车查验MTC")},
        {"032",QString::fromUtf8("入口重放行查验")},
        {"033",QString::fromUtf8("追缴名单ETC")},
        {"034",QString::fromUtf8("超六轴拦截ETC")},
        {"035",QString::fromUtf8("超六轴拦截MTC")},
    };

    if(qmBusinessAuthText.contains(sBussinessAuthorizType))
        return qmBusinessAuthText.value(sBussinessAuthorizType);
    else
        return QString::fromUtf8("未知业务授权");
}

QString NS_CommonFunc::getFeeCalcTypeName(int nCalcType)
{
    static QMap<int,QString> qmCalcTypeText =
    {
//        {1,QString::fromUtf8("门架预计费")},
//        {2,QString::fromUtf8("在线计费")},
//        {3,QString::fromUtf8("介质计费")},
//        {4,QString::fromUtf8("最小费额")}
        {1,QString::fromUtf8("按obu内累计优惠后金额计费")},
        {3,QString::fromUtf8("按cpc卡内累计金额计费")},
        {4,QString::fromUtf8("省中心在线服务计费")},
        {5,QString::fromUtf8("部中心在线服务计费")},
        {6,QString::fromUtf8("按全网最小费额计费")},
        {7,QString::fromUtf8("门架预计费")},
        {11,QString::fromUtf8("开放式计费")}
    };

    if(qmCalcTypeText.contains(nCalcType))
        return qmCalcTypeText.value(nCalcType);
    else
        return QString::fromUtf8("未知");
}

QString NS_CommonFunc::getExitBusinessAuthReasonHuNan(QString sBussinessAuthorizType)
{
    static QMap<QString,QString> qmBusinessAuthText =
    {
        {"002",QString::fromUtf8("节假日放行确认")},
        {"003",QString::fromUtf8("绿通查验")},
        {"004",QString::fromUtf8("联合收割机查验")},

        {"008",QString::fromUtf8("黑名单车辆")},
        {"009",QString::fromUtf8("最小费额授权")},

        {"013",QString::fromUtf8("超时车异常拦截")},

        {"015",QString::fromUtf8("U转车查验")},
        {"016",QString::fromUtf8("大件运输车辆查验")},

        {"018",QString::fromUtf8("大件运输车辆查验")},

        {"020",QString::fromUtf8("ETC车型车牌校验")},

        {"022",QString::fromUtf8("无治超数据人工确认")},
        {"023",QString::fromUtf8("入出口称重信息人工确认")},
        {"024",QString::fromUtf8("货车超限人工确认")},
        {"025",QString::fromUtf8("已交易放行查验")},
        {"026",QString::fromUtf8("入出口轴数不符查验")},
        {"027",QString::fromUtf8("计费超过上限需要集中监控确认")},
        {"028",QString::fromUtf8("使用最小费额需要集中监控确认")},
        {"029",QString::fromUtf8("ETC单卡入出口车型不符确认")},
        {"030",QString::fromUtf8("专项作业车查验")},
        {"031",QString::fromUtf8("超6轴车查验")},
        {"032",QString::fromUtf8("计费超过上限需要集中监控确认")},

    };

    if(qmBusinessAuthText.contains(sBussinessAuthorizType))
        return qmBusinessAuthText.value(sBussinessAuthorizType);
    else
        return QString::fromUtf8("未知业务授权");
}

void NS_CommonFunc::delayMs(int milliseconds)
{
    QEventLoop loop;
    QTimer::singleShot(milliseconds, &loop, &QEventLoop::quit);
    loop.exec();
}
