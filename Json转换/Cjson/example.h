#include "json.h"
#include "GeneralDefine.h"//自己定义的一些
/**
	实例演示：
*/


struct tagRspVehQTypeParam
{
	struct tagVehTypeQueue
	{
		string RegId;                   // 车型识别Id 数字
		string RegTime;                 // 识别时间 yyyy-MM-dd hh:mm:ss.zzz
	};
	string RegVehTypeNo;                // 设备编号
	string SerialNo;                    // 命令序列号 数字，自增，0~999999
	vector<tagVehTypeQueue> content;    // 车型队列
};

// 生成json串
bool generateJson(string &strCmd, string &strRsp);

// 解析json串
bool parseJsonData(const string & strJson, string & strStatusCode);

//结构体里面有数组，转json
string Stuct2Json(tagRspVehQTypeParam &tagParams);
