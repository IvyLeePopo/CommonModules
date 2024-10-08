/*
 * Copyright(C) 2014,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 *
 * 文件名称：SQLite3Base.h
 * 摘    要：包含SQLite3的基本操作
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2014年8月6日
 */
#pragma once
#include "sqlite3.h"
//#pragma comment(lib, "sqlite3.lib")

class CSQLite3
{
public:
	CSQLite3(void);
	~CSQLite3(void);
//Interfaces
public:
	//获取最新的错误信息
	inline const CString GetLastErrorDesc() {return m_strLastErrorDesc;}
	inline const CString GetLastErrorDesc(IN OUT DWORD& dwErrorCode) {dwErrorCode = m_dwLastErrorCode; return m_strLastErrorDesc;}

	//设置数据库文件的绝对路径
	inline const BOOL SetDatabaseFilePath(IN const CString& strFilePath) {m_strDatabaseFilePath = strFilePath;}

	//获取上一次操作的消耗时间
	inline const DWORD GetLastOperationConsumeTime(IN const int& iType = 1) {return (1 == iType) ? m_dwExecuteSQLConsumeTime : m_dwExecuteTransactionTime;}

	/***************************标准SQL执行*************************************/

	//执行SQL语句
	const BOOL ExecuteSQL(IN const CString& strSQL, IN const BOOL& bIgnorePrimaryKey = FALSE);
	//执行SQL语句与调用函数
	const BOOL ExecuteSQL(IN const CString& strSQL, int (*callback)(void*, int, char**, char**), IN const BOOL& bIgnorePrimaryKey = FALSE);
	//开启事务
	const BOOL BeginTransaction(IN const DWORD& dwSingleSubmitCount = 0);
	//递交事务
	const BOOL CommitTransaction(void);
	//回滚事务
	const BOOL RollbackTransaction(void);

	/***************************系统操作*************************************/

	//设置打开数据库的属性标记
	void SetOpenConnectFlag(IN const BOOL& bMulThread = FALSE);
	//打开数据库
	const BOOL OpenConnection(IN const BOOL& bAutoShrinkageDatabase = FALSE);
	const BOOL OpenConnection(IN const CString& strDatabaseFilePath, IN const BOOL& bAutoShrinkageDatabase = FALSE);
	//关闭数据库
	const BOOL CloseConnection(void);
	//打开记录集
	const BOOL OpenRecordSet(void);
	//关闭记录集
	const BOOL CloseRecordSet(void);
	//判断链接是否打开
	inline const BOOL IsOpenConnection(void) const {return NULL != m_pSQLite3DB;}
	//判断记录集是否打开
	inline const BOOL IsOpenRecordSet(void) const {return NULL != m_pSQLite3STMT;}


	//检索给定表是否存在
	const BOOL CheckTableExists(IN const CString& strTableName);
	//删除表
	const BOOL DropTable(IN const CString& strTableName);

	//多少行数据被影响
	const int EffectRows(IN const CString& strSQL = _T(""));
	//获取当前数据系统内存使用状态
	const sqlite3_int64 GetUseMemory(void);
	//获取黑名单本地文件实时大小
	const DWORD GetDatabaseFileSize(void);

	/***************************记录集数据获取*************************************/

	//判断当前返回记录集是否已经游标到最后
	const BOOL IsEOF(void);
	//根据列名获取其值(字符型)
	const BOOL GetColumnValue(IN const CString& strColumnName, IN OUT CString& strValue);
	const BOOL GetColumnValue(IN const CString& strColumnName, IN OUT TCHAR* szValue);
	//根据列名获取其值(有符号4字节整型)
	const BOOL GetColumnValue(IN const CString& strColumnName, IN OUT int& iValue);
	//根据列名获取其值(无符号4字节整型)
	const BOOL GetColumnValue(IN const CString& strColumnName, IN OUT UINT& nValue);
	//根据列名获取其值(有符号8字节整型)
	const BOOL GetColumnValue(IN const CString& strColumnName, IN OUT INT64& llValue);
	//根据列名获取其值(浮点型)
	const BOOL GetColumnValue(IN const CString& strColumnName, IN OUT double& dValue);
	//根据列名获取其值(日期型)
	const BOOL GetColumnValue(IN const CString& strColumnName, IN OUT CTime& cTime);
private:
	/**
	* add zhuyabing 2017-04
	* 在VS调试的情况下，如果出现了 throw语句，会出现频繁出现提示信息，影响调测
	* 此处对可能影响的部分，统一增加没有异常的版本
	*/
	const BOOL CloseRecordSet_noexcept(void);
	const BOOL CloseConnection_noexcept(void);
private:
	//设置打开数据库的属性标记
	void SetOpenConnectFlag(IN const DWORD& dwOpenFlag) { m_dwOpenFlag = dwOpenFlag;}
	//ASCII字符串转化为Unicode字符串
	const CString AsciiToUnicode(IN const char* szString);
	//Unicode字符串转化为ASCII字符串
	char* UnicodeToAscii(IN const CString& strString);
	//收缩数据库
	const BOOL AutoShrinkageDatabase(void);
//Attributes
private:
	//检测数据库句柄是否已经被初始化
	BOOL	m_bInitDatabase;
	//是否忽略主键冲突
	BOOL	m_bIgnorePrimaryKey;
	//最新的错误代码
	DWORD	m_dwLastErrorCode;
	//最新的错误描述信息
	CString m_strLastErrorDesc;
	//本地数据库文件绝对路径
	CString	m_strDatabaseFilePath;
	//数据句柄
	sqlite3* m_pSQLite3DB;
	//数据集句柄
	sqlite3_stmt* m_pSQLite3STMT;
	//打开数据库的标记--默认创建、只读属性
	DWORD	m_dwOpenFlag;
	//执行SQL影响的记录数
	DWORD	m_dwEffectRecordCount;
	//获取记录集数据索引
	DWORD	m_dwIndexOfRecordSet;
	//标记执行SQL后是否有记录集
	BOOL	m_bConfirmHasRecordSet;
	//在设置事务的时候，执行插入语句时积累到指定记录数则进行递交
	DWORD	m_dwCommitInsertRecordCount;
	//开始执行事务的初始时间
	DWORD	m_dwStartTransactionTime;
	//执行单步SQL操作的消耗时间
	DWORD	m_dwExecuteSQLConsumeTime;
	//执行事务操作消耗的时间
	DWORD	m_dwExecuteTransactionTime;


    
};
