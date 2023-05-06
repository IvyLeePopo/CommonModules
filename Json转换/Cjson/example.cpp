#include "json.h"
#include "GeneralDefine.h"//自己定义的一些
/**
	实例演示：
*/

// 字符串或者数组---> 组建json

/**
	strCmd：传进来的字符串
	strRsp：构建成的json串
*/
bool generateJson(string &strCmd, string &strRsp)
{
	CHECKJSON(strCmd);
	Json::Value jsonRsp;
	GETSTRINGFROMJSON("CameraDevNo", jsonRoot, jsonRsp["CameraDevNo"]);
	jsonRsp["Result"] = "0";
	jsonRsp["ResultTime"] = "";

	Json::FastWriter writer;
	strRsp = writer.write(jsonRsp);
	return true;
}

/**
	strJson：传进来的字符串
	strStatusCode：读取json串里面的数据
*/
bool parseJsonData(const string & strJson, string & strStatusCode)
{
	CHECKJSON(strJson);
	// 获得一个字符串
	GETSTRINGFROMJSON("StatusCode", jsonRoot, strStatusCode);

	Json::Value jsonPlateResult;
	// 获得一个节点
	GETNODEFROMJSON("PlateResult", jsonRoot, jsonPlateResult);
	{
		GETINTFROMJSON("carColor", jsonPlateResult, params.nColor);
		GETINTFROMJSON("colorType", jsonPlateResult, params.nColorType);
		GETINTFROMJSON("colorValue", jsonPlateResult, params.nColorValue);
		GETWSTRINGFROMJSON("license", jsonPlateResult, params.strPlate);
	}

	return false;
}

