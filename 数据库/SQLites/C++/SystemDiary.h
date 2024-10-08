#ifndef SYSTEM_DIARY_H
#define SYSTEM_DIARY_H


#include <string>
#include <map>
#include <list>
#include "TypeHelper.h"

#define TW_TRACE_TYPE_INVOKING  1    //函数调用

#define TW_TRACE_TPPE_INTERFACE  101    //接口调用单元
#define TW_TRACE_TPPE_PAYUNIT  102      //支付处理单元
#define TW_TRACE_TPPE_PAYHTTP  103      //支付处理单元
#define TW_TRACE_TPPE_INVOICEUNIT  104  //发票处理单元
#define TW_TRACE_TPPE_INVOICEHTTP  105  //发票处理单元

#define TW_TRACE_TYPE_INTERFACE  201    //接口调用单元
#define TW_TRACE_TYPE_ETCMANAGE  202    //ETC请求处理单元
#define TW_TRACE_TYPE_ETCPAYUNIT 203    //ETC支付处理
#define TW_TRACE_TYPE_ETCAUTH    204    //白名单查询单元

#define TW_TRACE_TYPE_DEBUGVALUE 9999999    //变量跟踪


#define TW_TRACE_DIARY(loginfo) (sys_trace_info((loginfo)))
#define TW_TRACE_DIARY_TYPE(loginfo,msgType) (sys_trace_info((loginfo),(msgType)))

#if 1
#define TW_DEBUG_VARIABLE(var) (sys_debug_var((#var),var))
#endif


void sys_trace_info(const std::string& some_info,int msgType = 0 );
const std::string GetCurTimeStr(void);
void sys_debug_var(const std::string& var_name,int var);
void sys_debug_var(const std::string& var_name,std::string var);
void sys_debug_var(const std::string& var_name,SYSTEMTIME var);
void sys_debug_var(const std::string& var_name,std::map<std::string,void*> dict);
void sys_debug_var(const std::string& var_name,std::list<std::string> str_list);


/*接口调用信息跟踪*/
class CInterfaceTraceInfo
{
public:
    CInterfaceTraceInfo(const std::string& name, const std::string & para = "")
    {
        loc_name = name;
        std::string loginfo = "接口调用开始:(" + loc_name + ")" + para;
        GetSystemTime(&call_time);
        TW_TRACE_DIARY_TYPE(loginfo, TW_TRACE_TYPE_INTERFACE);
    }
    ~CInterfaceTraceInfo()
    {
        SYSTEMTIME curr_time;
        GetSystemTime(&curr_time);
        int cost_ms = static_cast<int>(GetDiffMilliSeconds(call_time, curr_time));
        char log_info[100] = {0};
        _snprintf(log_info,80, "接口调用结束:%s,耗时:%dms", loc_name.c_str(), cost_ms);
        TW_TRACE_DIARY_TYPE(log_info, TW_TRACE_TYPE_INTERFACE);
    }
public:
    SYSTEMTIME call_time;
    std::string loc_name;
private:
    CInterfaceTraceInfo(const CInterfaceTraceInfo&);
    CInterfaceTraceInfo& operator = (const CInterfaceTraceInfo&);
};


/*接口调用信息跟踪*/
class CFunctionCallTrace
{
public:
    CFunctionCallTrace(const std::string& name, const std::string & para = "")
    {
        loc_name = name;
        std::string loginfo = "调用开始:(" + loc_name + ")" + para;
        GetSystemTime(&call_time);
        TW_TRACE_DIARY_TYPE(loginfo, TW_TRACE_TYPE_INVOKING);
    }
    ~CFunctionCallTrace()
    {
        SYSTEMTIME curr_time;
        GetSystemTime(&curr_time);
        int cost_ms = static_cast<int>(GetDiffMilliSeconds(call_time, curr_time));
        char log_info[100] = {0};
        _snprintf(log_info,80, "调用结束:%s,耗时:%dms", loc_name.c_str(), cost_ms);
        TW_TRACE_DIARY_TYPE(log_info, TW_TRACE_TYPE_INVOKING);
    }
public:
    SYSTEMTIME call_time;
    std::string loc_name;
private:
    CFunctionCallTrace(const CFunctionCallTrace&);
    CFunctionCallTrace& operator = (const CFunctionCallTrace&);
};


#endif // SYSTEM_DIARY_H
