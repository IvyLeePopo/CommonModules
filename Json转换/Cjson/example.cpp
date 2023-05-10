#include "example.h"
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

//结构体里面有数组，转json
string Stuct2Json(tagRspVehQTypeParam &tagParams)
{
	Json::Value root;
	Json::FastWriter writer;
	//指令应答头部
	root["RegVehTypeNo"] = Json::Value(tagParams.RegVehTypeNo);
	root["SerialNo"] = Json::Value(tagParams.SerialNo);
	//数组的个数
	size_t nNum = tagParams.content.size();
	Json::Value contentRoot, contentRootObj;
	for (size_t index = 0; index < nNum; ++index)
	{
		contentRootObj["RegId"] = Json::Value(tagParams.content.at(index).RegId);
		contentRootObj["VehPlate"] = Json::Value(tagParams.content.at(index).VehPlate);
		contentRoot.append(contentRoot);
	}
	root["VehTypeQueue"] = contentRoot;
	return writer.write(root);
}

