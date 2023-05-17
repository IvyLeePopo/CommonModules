#include "sdk_example.h"

// 回调函数
void _CBFun_GetRegResult(int nHandle, T_VLPINFO* pVlpResult, void *pUser)
{
	printf("_CBFun_GetRegResult识别结果回调！\n");//动态库主动调用的接口
}

void loadDll()
{
	//1、加载一个动态库：
	HMODULE m_module = ::LoadLibraryA("VPR_Trawe_V1001.dll");
	m_vlprLogin = (vlprLogin)::GetProcAddress(m_module, "VLPR_Login");//可以直接调用这接口

	// 2、设置回调函数
	m_vlprSetResultCallBack = (vlprSetResultCallBack)::GetProcAddress(m_module, "VLPR_SetResultCallBack");//加载动态库接口
	m_vlprSetResultCallBack(m_nDeviceHandle, _CBFun_GetRegResult, this);//设置具体的参数：把外部的回调函数也设置好
}

// 在动态库里面：如何回调？
// 动态库接收外部设置的回调函数

typedef void(*CBFun_GetRegResult)(int nHandle, T_VLPINFO* pVlpResult, void *pUser);
CBFun_GetRegResult m_pCBGetRegResult;       // 识别结果回调成员

int CameraControlApp::vlprSetResultCallBack(int nHandle, CBFun_GetRegResult pFunc, void* pUser)
{
	m_pCBGetRegResult = pFunc;
}
// 调用外部的回调函数
m_pCBGetRegResult(m_nHandle, &pParams->vlpInfo, m_pUser);

