/*
 * Copyright(C) 2014,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 *
 * 文件名称：SQLite3Base.cpp
 * 摘    要：基于SQLite3的轻量级数据库基本操作类实现文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2014年8月6日
 */
#include "StdAfx.h"
#include "SQLite3Base.h"
#include "Shlwapi.h"

#include "sqlite3_agent.h"

CSQLite3::CSQLite3(void)
	: m_bInitDatabase(FALSE)
	, m_bIgnorePrimaryKey(FALSE)
	, m_dwLastErrorCode(0)
	, m_strLastErrorDesc(_T(""))
	, m_strDatabaseFilePath(_T(""))
	, m_pSQLite3DB(NULL)
	, m_pSQLite3STMT(NULL)
	, m_dwOpenFlag(SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE)
	, m_dwEffectRecordCount(0)
	, m_dwIndexOfRecordSet(0)
	, m_bConfirmHasRecordSet(FALSE)
	, m_dwCommitInsertRecordCount(0)
	, m_dwStartTransactionTime(0)
	, m_dwExecuteSQLConsumeTime(0)
	, m_dwExecuteTransactionTime(0)
{
	
}

CSQLite3::~CSQLite3(void)
{
	if (IsOpenRecordSet())
	{
		CloseRecordSet();
	}
	if (IsOpenConnection())
	{
		CloseConnection();
	}
	
}

// ========================================================================================================================
//
// 函数名称: CSQLite3::SetOpenConnectFlag()
//
// 函数功能: 设置打开数据库的标记
// 输入参数: IN const BOOL& bMulThread -- 是否允许多线程
// 返回参数: void
// 函数版本: 1.0.0.1
// 编写人员: [ROCY]
// 编写时间: [2015-1-9 15:28:48]
// 备注说明: 必须在调用OpenConnection之前调用方有效
//
// ========================================================================================================================
void CSQLite3::SetOpenConnectFlag(IN const BOOL& bMulThread)
{
	DWORD dwOpenFlag = SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE;
	if (bMulThread)
		dwOpenFlag |= SQLITE_OPEN_FULLMUTEX;
	SetOpenConnectFlag(dwOpenFlag);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSQLite3::ExecuteSQL
//
// 功能描述：执行SQL语句
//
// 输入参数：IN const CString& strSQL		   -- 是否忽略主键冲突
//		   ：IN const BOOL& bIgnorePrimaryKey  -- 是否忽略主键冲突
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2014年8月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::ExecuteSQL(IN const CString& strSQL, IN const BOOL& bIgnorePrimaryKey)
{
	BOOL bResult = FALSE;
	char szThrowDBError[512] = {0};
	CString strRealSQL(strSQL);
	strRealSQL.TrimLeft();
	strRealSQL.TrimRight();
	try
	{
		//判断数据库是否有效
		if (!IsOpenConnection())
			throw - 1;
		//判断是执行什么语句
		BOOL bHasRecordSet = FALSE;
		BOOL bContinueExecuteSQL = TRUE;
		int iDBStatus = -1;
		if (0 == strSQL.Left(6).CompareNoCase(_T("SELECT")))
		{
			if (NULL != m_pSQLite3STMT)
			{
				iDBStatus = sqlite3_agent::sqlite3_finalize_fun(m_pSQLite3STMT);
				m_pSQLite3STMT = NULL;
			}
			//尝试多次执行指定SQL
			while (bContinueExecuteSQL)
			{
				iDBStatus = sqlite3_agent::sqlite3_prepare_v2_fun(m_pSQLite3DB, strRealSQL.GetBuffer(0), strRealSQL.GetLength(), &m_pSQLite3STMT, NULL);
				switch (iDBStatus)
				{
				case SQLITE_BUSY:
					::Sleep(50);
					break;
				case SQLITE_OK:
					bContinueExecuteSQL = FALSE;
					break;
				default:
					bContinueExecuteSQL = FALSE;
					break;
				}
				Sleep(50);
			}
			//标记有记录集返回
			m_bConfirmHasRecordSet = TRUE;
		}
		else
		{
			m_bConfirmHasRecordSet = FALSE;

			while (bContinueExecuteSQL)
			{
				iDBStatus = sqlite3_agent::sqlite3_exec_fun(m_pSQLite3DB, strRealSQL.GetBuffer(0), NULL, NULL, NULL);
				switch (iDBStatus)
				{
				case SQLITE_BUSY:
					::Sleep(50);
					break;
				case SQLITE_OK:
					bContinueExecuteSQL = FALSE;
					break;
				default:
					bContinueExecuteSQL = FALSE;
					break;
				}
				Sleep(50);
			}
		}

		//判断异常 --  如果不需要忽略主键冲突，则抛出异常
		if (19 == iDBStatus)
		{
			if (!bIgnorePrimaryKey)
			{
				if (SQLITE_OK != iDBStatus)
				{
					throw - 3;
				}
			}
		}
		else if (SQLITE_OK != iDBStatus)
			throw - 3;

		//获得影响记录数--插入或删除
		m_dwEffectRecordCount = EffectRows(m_bConfirmHasRecordSet ? strSQL : _T(""));
		//如果返回有记录集，则单独处理
		if (m_bConfirmHasRecordSet)
		{
			m_dwIndexOfRecordSet = 0;
		}
		bResult = TRUE;
	}
	catch (int& iErrorCode)
	{
		//设置记录集标记为假
		m_bConfirmHasRecordSet = FALSE;
		//获得数据库反馈的错误与代码
		m_dwLastErrorCode = sqlite3_agent::sqlite3_errcode_fun(m_pSQLite3DB);
		strcpy_s(szThrowDBError, _countof(szThrowDBError), sqlite3_agent::sqlite3_errmsg_fun(m_pSQLite3DB));
		switch (iErrorCode)
		{
		case -1:
			m_strLastErrorDesc.Format(_T("给定的SQLite3数据库句柄无效!"));
			break;
		case -2:
			m_strLastErrorDesc.Format(_T("字符串转换失败!"));
			break;
		case -3:
			m_strLastErrorDesc.Format(_T("SQL执行失败,错误代码:%d,错误描述:%s"), m_dwLastErrorCode, szThrowDBError);
			break;
		}
	}
	return bResult;
}

const BOOL CSQLite3::ExecuteSQL(IN const CString& strSQL, int (*callback)(void*, int, char**, char**), IN const BOOL& bIgnorePrimaryKey)
{
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSQLite3::BeginTransaction
//
// 功能描述：开启事务
//
// 输入参数：IN const DWORD& dwSingleSubmitCount=0
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2014年8月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::BeginTransaction(IN const DWORD& dwSingleSubmitCount)
{
	BOOL bResult = FALSE;
	bResult = ExecuteSQL(_T("BEGIN;"));
	if (bResult)
	{
		m_dwCommitInsertRecordCount = dwSingleSubmitCount;
		m_dwStartTransactionTime = ::GetTickCount();
	}
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSQLite3::CommitTransaction
//
// 功能描述：递交事务
//
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2014年8月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::CommitTransaction(void)
{
	BOOL bResult = FALSE;
	bResult = ExecuteSQL(_T("COMMIT;"));
	if (bResult)
	{
		m_dwCommitInsertRecordCount = -1;
		m_dwExecuteTransactionTime = ::GetTickCount() - m_dwStartTransactionTime;
	}
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSQLite3::RollbackTransaction
//
// 功能描述：回滚事务
//
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2014年8月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::RollbackTransaction(void)
{
	BOOL bResult = FALSE;
	bResult = ExecuteSQL(_T("ROLLBACK;"));
	if (bResult)
	{
		m_dwCommitInsertRecordCount = -1;
		m_dwExecuteTransactionTime = ::GetTickCount() - m_dwStartTransactionTime;
	}
	return bResult;
}

	

char* TMP_MbcsToUtf8(const char *file)
{
    CString str;
    WCHAR   *pwchar=0;
    CHAR    *pchar=0;
    int len=0;
    int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
    len=MultiByteToWideChar(codepage, 0, file, -1, NULL,0);
    pwchar=new WCHAR[len];
    if(pwchar!=0)
    {
        len = MultiByteToWideChar(codepage, 0, file, -1, pwchar, len);
        if( len!=0 ) 
        {
            len = WideCharToMultiByte(CP_UTF8, 0, pwchar, -1, 0, 0, 0, 0);
            pchar=new CHAR[len];
            if(pchar!=0)
            {
                len = WideCharToMultiByte(CP_UTF8, 0, pwchar, -1, pchar, len,0, 0);
                if(len!=0)                 
                {
                    str=pchar;                    
                }
                delete pchar;
            }
            delete pwchar; 
        }
    }

	int iLen = str.GetLength();
	char* pszData = NULL;
	if (iLen > 0)
	{
		pszData = new char[iLen + 1];
		if (NULL != pszData)
		{
			memset(pszData, 0, iLen + 1);
			strcpy(pszData, str.GetBuffer(0));
		}
	}
	return pszData;
	

}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSQLite3::OpenConnection
//
// 功能描述：打开一个数据库，不存在则创建
//
// 输入参数：IN const BOOL& bAutoShrinkageDatabase	--	是否设置数据库自动收缩
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2014年8月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::OpenConnection(IN const BOOL& bAutoShrinkageDatabase)
{
	BOOL bResult = FALSE;
	char szThrowDBError[512] = {0};
	try
	{
		//检查内部文件绝对路径是否为空
		if (m_strDatabaseFilePath.IsEmpty())
			throw - 1;
		//检查数据库句柄是否已经有效(有效则去除)
		int iDBStatus = -1;
		if (!CloseConnection())
			throw - 2;
		//字符串转化
		//modify 
		char* pszString = TMP_MbcsToUtf8((LPSTR)(LPCTSTR)m_strDatabaseFilePath);// = UnicodeToAscii(m_strDatabaseFilePath);
		if (NULL == pszString)
			throw - 3;
		//执行创建或打开SQLite3数据库指令
		iDBStatus = sqlite3_agent::sqlite3_open_v2_fun(pszString, &m_pSQLite3DB, m_dwOpenFlag, NULL);
		if (SQLITE_OK != iDBStatus)
			throw - 4;
		if (bAutoShrinkageDatabase)
		{
			if (!AutoShrinkageDatabase())
			{
				throw - 5;
			}
		}
		//清除资源
		delete []pszString;
		pszString = NULL;

		bResult = TRUE;
	}
	catch (int& iErrorCode)
	{
		//获得数据库反馈的错误与代码
		m_dwLastErrorCode = sqlite3_agent::sqlite3_errcode_fun(m_pSQLite3DB);
		strcpy_s(szThrowDBError, _countof(szThrowDBError), sqlite3_agent::sqlite3_errmsg_fun(m_pSQLite3DB));
		switch (iErrorCode)
		{
		case -1:
			m_strLastErrorDesc.Format(_T("内部错误:类指定的数据库文件绝对路径字符值为空!"));
			break;
		case -2:
			break;//内部已经处理
		case -3:
			m_strLastErrorDesc.Format(_T("内部错误:字符串转化(ASCII转Unicode)失败!"));
			break;
		case -4:
			m_strLastErrorDesc.Format(_T("内部错误:打开数据库文件失败,错误代码:%d,错误描述:%s"), m_dwLastErrorCode, szThrowDBError);
			break;
		case -5:
			//内部已经处理
			break;
		}
	}
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSQLite3::OpenConnection
//
// 功能描述：打开一个数据库，不存在则创建
//
// 输入参数：IN const CString& strDatabaseFilePath	--	打开的数据库文件绝对路径
//		   ：IN const BOOL& bAutoShrinkageDatabase	--	是否设置数据库自动收缩
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2014年8月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::OpenConnection(IN const CString& strDatabaseFilePath, IN const BOOL& bAutoShrinkageDatabase)
{
	m_strDatabaseFilePath = strDatabaseFilePath;
	return OpenConnection(bAutoShrinkageDatabase);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSQLite3::CloseConnection
//
// 功能描述：关闭数据库链接
//
// 输入参数：void
// 输出参数：const BOOL
// 编写人员：ROCY
// 编写时间：2014年8月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::CloseConnection(void)
{
	return CloseConnection_noexcept();
	BOOL bResult = TRUE;
	int iDBStatus = -1;
	char szThrowDBError[512] = {0};
	try
	{
		if (NULL != m_pSQLite3DB)
		{
			iDBStatus = sqlite3_agent::sqlite3_close_v2_fun(m_pSQLite3DB);
			if (SQLITE_OK != iDBStatus)
				throw - 1;
			m_pSQLite3DB = NULL;
		}
	}
	catch (int& iErrorCode)
	{
		//获得数据库反馈的错误与代码
		m_dwLastErrorCode = sqlite3_agent::sqlite3_errcode_fun(m_pSQLite3DB);
		strcpy_s(szThrowDBError, _countof(szThrowDBError), sqlite3_agent::sqlite3_errmsg_fun(m_pSQLite3DB));
		switch (iErrorCode)
		{
		case -1:
			m_strLastErrorDesc.Format(_T("内部错误:关闭已经存在的SQLite3数据库句柄失败!"));
			break;
		}
		//防御性代码,当关闭失败的时候,直接将该数据库文件句柄抛弃
		if (SQLITE_ERROR == iDBStatus)
		{
			m_pSQLite3DB = NULL;
		}
		//返回失败
		bResult = FALSE;
	}
	return bResult;
}
const BOOL CSQLite3::CloseConnection_noexcept(void)
{
	BOOL bResult = TRUE;
	int preThrowNo = 0;
	int iDBStatus = -1;
	char szThrowDBError[512] = {0};
	while (true)
	{
		if (NULL != m_pSQLite3DB)
		{
			iDBStatus = sqlite3_agent::sqlite3_close_v2_fun(m_pSQLite3DB);
			if (SQLITE_OK != iDBStatus)
			{
				preThrowNo = - 1;
				break;
			}
			m_pSQLite3DB = NULL;
		}
		break;
	}
	if (preThrowNo != 0)
	{
		//获得数据库反馈的错误与代码
		m_dwLastErrorCode = sqlite3_agent::sqlite3_errcode_fun(m_pSQLite3DB);
		strcpy_s(szThrowDBError, _countof(szThrowDBError), sqlite3_agent::sqlite3_errmsg_fun(m_pSQLite3DB));
		switch (preThrowNo)
		{
		case -1:
			m_strLastErrorDesc.Format(_T("内部错误:关闭已经存在的SQLite3数据库句柄失败!"));
			break;
		}
		//防御性代码,当关闭失败的时候,直接将该数据库文件句柄抛弃
		if (SQLITE_ERROR == iDBStatus)
		{
			m_pSQLite3DB = NULL;
		}
		//返回失败
		bResult = FALSE;
	}


	return bResult;

}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSQLite3::OpenRecordSet
//
// 功能描述：打开上一次执行SQL语句返回的结果记录集
//
// 输入参数：void
// 输出参数：const BOOL
// 编写人员：ROCY
// 编写时间：2014年8月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::OpenRecordSet(void)
{
	if (    (m_bConfirmHasRecordSet)
	        && (NULL != m_pSQLite3STMT)
	   )
	{
		return TRUE;
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSQLite3::CloseRecordSet
//
// 功能描述：关闭记录集
//
// 输入参数：void
// 输出参数：const BOOL
// 编写人员：ROCY
// 编写时间：2014年8月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::CloseRecordSet(void)
{
	return CloseRecordSet_noexcept();
	BOOL bResult = TRUE;
	int iDBStatus = -1;
	char szThrowDBError[512] = {0};
	try
	{
		if (!IsOpenConnection())
			throw - 1;
		if (NULL != m_pSQLite3STMT)
		{
			iDBStatus = sqlite3_agent::sqlite3_finalize_fun(m_pSQLite3STMT);
			if (SQLITE_OK != iDBStatus)
				throw - 2;
			m_pSQLite3STMT = NULL;
		}
	}
	catch (int& iErrorCode)
	{
		//获得数据库反馈的错误与代码
		m_dwLastErrorCode = sqlite3_agent::sqlite3_errcode_fun(m_pSQLite3DB);
		strcpy_s(szThrowDBError, _countof(szThrowDBError), sqlite3_agent::sqlite3_errmsg_fun(m_pSQLite3DB));
		switch (iErrorCode)
		{
		case -1:
			m_strLastErrorDesc.Format(_T("内部错误:记录集关联的SQLite3数据库句柄无效!"));
			break;
		case -2:
			m_strLastErrorDesc.Format(_T("内部错误:关闭已经存在的SQLite3记录集句柄失败!"));
			break;
		}
		bResult = FALSE;
	}
	return bResult;
}
const BOOL CSQLite3::CloseRecordSet_noexcept(void)
{
	BOOL bResult = TRUE;
	int preThrowNo = 0;
	int iDBStatus = -1;
	char szThrowDBError[512] = {0};
	while (true)
	{
		if (!IsOpenConnection())
		{
			preThrowNo = -1;
			break;
		}
		if (NULL != m_pSQLite3STMT)
		{
			iDBStatus = sqlite3_agent::sqlite3_finalize_fun(m_pSQLite3STMT);
			if (SQLITE_OK != iDBStatus)
			{
				preThrowNo = -2;
				break;
			}
			m_pSQLite3STMT = NULL;
		}
		break;
	}
	if (preThrowNo != 0)
	{
		//获得数据库反馈的错误与代码
		m_dwLastErrorCode = sqlite3_agent::sqlite3_errcode_fun(m_pSQLite3DB);
		strcpy_s(szThrowDBError, _countof(szThrowDBError), sqlite3_agent::sqlite3_errmsg_fun(m_pSQLite3DB));
		switch (preThrowNo)
		{
		case -1:
			m_strLastErrorDesc.Format(_T("内部错误:记录集关联的SQLite3数据库句柄无效!"));
			break;
		case -2:
			m_strLastErrorDesc.Format(_T("内部错误:关闭已经存在的SQLite3记录集句柄失败!"));
			break;
		}
		bResult = FALSE;
	}
	return bResult;

}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSQLite3::CheckTableExists
//
// 功能描述：检测给定的表是否存在
//
// 输入参数：IN const CString& strTableName -- 表名
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2014年8月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::CheckTableExists(IN const CString& strTableName)
{
	BOOL bRsult = FALSE;
	try
	{
		if (!IsOpenConnection())
			throw - 1;
		//执行查询SQL
		CString strSQL;
		strSQL.Format(_T("SELECT tbl_name FROM sqlite_master WHERE type='table' "));
		//关闭已经存在的记录集
		CloseRecordSet();
		//执行查询语句
		if (!ExecuteSQL(strSQL))
			throw - 2;
		if (NULL == m_pSQLite3STMT)
			throw - 3;
		while (!IsEOF())
		{
			CString strValue;
			if (!GetColumnValue(_T("tbl_name"), strValue))
				throw - 4;
			if (strTableName == strValue)
			{
				bRsult = TRUE;
				break;
			}
			Sleep(1);
		}
		//关闭数据集
		CloseRecordSet();
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			m_strLastErrorDesc.Format(_T("内部错误:SQLite3数据库链接句柄无效!"));
			break;
		case -2://已经内部处理
		case -3://已经内部处理
			break;
		case -4:
			break;
		case -5://检索行数据出错
			break;
		}
	}
	return bRsult;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSQLite3::DropTable
//
// 功能描述：删除一张已经存在的用户表
//
// 输入参数：IN const CString& strTableName -- 表名
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2014年8月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::DropTable(IN const CString& strTableName)
{
	BOOL bResult = TRUE;
	try
	{
		if (!CheckTableExists(strTableName))
			throw - 1;
		CString strSQL;
		strSQL.Format(_T("DROP TABLE %s"), strTableName);
		if (!ExecuteSQL(strSQL))
			throw - 2;
		bResult = TRUE;
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
		case -2:
			break;
		}
	}
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSQLite3::EffectRows
//
// 功能描述：获取多少行记录被影响
//
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2014年8月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
const int CSQLite3::EffectRows(IN const CString& strExecuteSQL)
{
	int iEffectRows = -1;
	int iDBStatus = -1;
	CString strSQL(strExecuteSQL);
	if (IsOpenConnection())
	{
		if (strExecuteSQL.IsEmpty())
		{
			iEffectRows = sqlite3_agent::sqlite3_changes_fun(m_pSQLite3DB);
		}
		else
		{
			CString strTemp(strExecuteSQL);
			strTemp.MakeUpper();

			CString strWhere = strTemp.Right(strTemp.GetLength() - strTemp.Find(_T("FROM")));

			CString strColumnName(_T("RecordCount"));
			strSQL.Format(_T("SELECT COUNT(1) AS %s %s "), strColumnName, strWhere);

			sqlite3_stmt * pSQLite3STMT = NULL;

			if (SQLITE_OK == (iDBStatus = sqlite3_agent::sqlite3_prepare_v2_fun(m_pSQLite3DB, strSQL.GetBuffer(0), strSQL.GetLength(), &pSQLite3STMT, NULL)))
			{
				if (SQLITE_ROW == (iDBStatus = sqlite3_agent::sqlite3_step_fun(pSQLite3STMT)))
				{
					int iRecordCount = -1;
					//获取指定表的字段数目
					int iFieldCount = sqlite3_agent::sqlite3_column_count_fun(pSQLite3STMT);
					//字段数目大于0,表示数据存在,开始解析读取
					for (int i = 0; i < iFieldCount; ++i)
					{
						char szFieldName[128] = {0};
						strcpy_s(szFieldName, _countof(szFieldName), sqlite3_agent::sqlite3_column_name_fun(pSQLite3STMT, i));
						if (0 == strColumnName.CompareNoCase(szFieldName))
						{
							int iDataType = sqlite3_agent::sqlite3_column_type_fun(pSQLite3STMT, i);
							if (SQLITE_INTEGER == iDataType)
							{
								iRecordCount = sqlite3_agent::sqlite3_column_int_fun(pSQLite3STMT, i);
								iEffectRows = iRecordCount;
							}
							break;
						}
					}
				}
			}
			//释放资源
			strSQL.ReleaseBuffer();
			//关闭记录集
			if (NULL != pSQLite3STMT)
				iDBStatus = sqlite3_agent::sqlite3_finalize_fun(pSQLite3STMT);
			pSQLite3STMT = NULL;
		}
	}
	return iEffectRows;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSQLite3::GetUseMemory
//
// 功能描述：获取当前SQLite3数据库使用内存
//
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2014年8月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
const sqlite3_int64 CSQLite3::GetUseMemory(void)
{
	return sqlite3_agent::sqlite3_memory_used_fun();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSQLite3::GetDatabaseFileSize
//
// 功能描述：获取数据库文件大小
//
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2014年8月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
const DWORD CSQLite3::GetDatabaseFileSize(void)
{
	DWORD dwSize = 0;
	CFileStatus status;
	if (::PathFileExists(m_strDatabaseFilePath))
	{
		CFile::GetStatus(m_strDatabaseFilePath, status);
		dwSize = (DWORD)status.m_size;
	}
	return dwSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSQLite3::IsEOF
//
// 功能描述：判断记录集是否已经到底
//
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2014年8月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::IsEOF(void)
{
	int iDBStatus = -1;
	if (SQLITE_ROW == (iDBStatus = sqlite3_agent::sqlite3_step_fun(m_pSQLite3STMT)))
	{
		if (m_dwIndexOfRecordSet < m_dwEffectRecordCount)
			++m_dwIndexOfRecordSet;
		return FALSE;
	}
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSQLite3::GetColumnValue
//
// 功能描述：获取列值 - 字符串
//
// 输入参数：IN const CString& strColumnName -- 列名
//         ：IN OUT CString& strValue		 --	数据值
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2014年8月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::GetColumnValue(IN const CString& strColumnName, IN OUT CString& strValue)
{
	BOOL bResult = FALSE;
	try
	{
		if (NULL == m_pSQLite3STMT)
			throw - 1;
		//获取指定表的字段数目
		int iFieldCount = sqlite3_agent::sqlite3_column_count_fun(m_pSQLite3STMT);
		//字段数目大于0,表示数据存在,开始解析读取
		if (0 == iFieldCount)
			throw - 2;
		for (int i = 0; i < iFieldCount; ++i)
		{
			char szFieldName[128] = {0};
			strcpy_s(szFieldName, _countof(szFieldName), sqlite3_agent::sqlite3_column_name_fun(m_pSQLite3STMT, i));
			if (0 == strColumnName.CompareNoCase(szFieldName))
			{
				int iDataType = sqlite3_agent::sqlite3_column_type_fun(m_pSQLite3STMT, i);
				if (SQLITE_TEXT == iDataType)
				{
					bResult = TRUE;
					strValue.Format(_T("%s"), sqlite3_agent::sqlite3_column_text_fun(m_pSQLite3STMT, i));
				}
				break;
			}
		}
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			break;
		}
	}
	return bResult;
}

// ========================================================================================================================
//
// 函数名称: BOOL CSQLite3::GetColumnValue()
//
// 功能描述：获取列值 - 字符串
//
// 输入参数：IN const CString& strColumnName	-- 列名
//         ：IN OUT TCHAR* szValue				--	数据值
// 返回参数: const
// 函数版本: 1.0.0.1
// 编写人员: [ROCY]
// 编写时间: [2014-12-4 8:44:51]
// 备注说明:
//
// ========================================================================================================================
const BOOL CSQLite3::GetColumnValue(IN const CString& strColumnName, IN OUT TCHAR* szValue)
{
	CString strValue;
	BOOL bReturn = GetColumnValue(strColumnName, strValue);
	if (bReturn)
		strcpy(szValue, strValue.GetBuffer(0));
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSQLite3::GetColumnValue
//
// 功能描述：获取列值 - 整型
//
// 输入参数：IN const CString& strColumnName	--  列名
//         ：IN OUT int& iValue					--	数据值
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2014年8月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::GetColumnValue(IN const CString& strColumnName, IN OUT int& iValue)
{
	BOOL bResult = FALSE;
	try
	{
		if (NULL == m_pSQLite3STMT)
			throw - 1;
		//获取指定表的字段数目
		int iFieldCount = sqlite3_agent::sqlite3_column_count_fun(m_pSQLite3STMT);
		//字段数目大于0,表示数据存在,开始解析读取
		if (0 == iFieldCount)
			throw - 2;
		for (int i = 0; i < iFieldCount; ++i)
		{
			char szFieldName[128] = {0};
			strcpy_s(szFieldName, _countof(szFieldName), sqlite3_agent::sqlite3_column_name_fun(m_pSQLite3STMT, i));
			if (0 == strColumnName.CompareNoCase(szFieldName))
			{
				int iDataType = sqlite3_agent::sqlite3_column_type_fun(m_pSQLite3STMT, i);
				if (SQLITE_INTEGER == iDataType)
				{
					iValue = sqlite3_agent::sqlite3_column_int_fun(m_pSQLite3STMT, i);

					bResult = TRUE;
				}
				break;
			}
		}
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			break;
		}
	}
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSQLite3::GetColumnValue
//
// 功能描述：根据列名获取其值(无符号4字节整型)
//
// 输入参数：IN const CString& strColumnName	--  列名
//         ：IN OUT UINT& nValue				--  4字节无符号整型
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年3月22日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::GetColumnValue(IN const CString& strColumnName, IN OUT UINT& nValue)
{
	BOOL bResult = FALSE;
	try
	{
		if (NULL == m_pSQLite3STMT)
			throw - 1;
		//获取指定表的字段数目
		int iFieldCount = sqlite3_agent::sqlite3_column_count_fun(m_pSQLite3STMT);
		//字段数目大于0,表示数据存在,开始解析读取
		if (0 == iFieldCount)
			throw - 2;
		for (int i = 0; i < iFieldCount; ++i)
		{
			char szFieldName[128] = {0};
			strcpy_s(szFieldName, _countof(szFieldName), sqlite3_agent::sqlite3_column_name_fun(m_pSQLite3STMT, i));
			if (0 == strColumnName.CompareNoCase(szFieldName))
			{
				int iDataType = sqlite3_agent::sqlite3_column_type_fun(m_pSQLite3STMT, i);
				if (SQLITE_INTEGER == iDataType)
				{
					nValue = (UINT)sqlite3_agent::sqlite3_column_int_fun(m_pSQLite3STMT, i);

					bResult = TRUE;
				}
				break;
			}
		}
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			break;
		}
	}
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSQLite3::GetColumnValue
//
// 功能描述：根据列名获取其值(有符号8字节整型)
//
// 输入参数：IN const CString& strColumnName	--  列名
//         ：IN OUT INT64& llValue				--  8字节有符号整型
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年3月22日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::GetColumnValue(IN const CString& strColumnName, IN OUT INT64& llValue)
{
	BOOL bResult = FALSE;
	try
	{
		if (NULL == m_pSQLite3STMT)
			throw - 1;
		//获取指定表的字段数目
		int iFieldCount = sqlite3_agent::sqlite3_column_count_fun(m_pSQLite3STMT);
		//字段数目大于0,表示数据存在,开始解析读取
		if (0 == iFieldCount)
			throw - 2;
		for (int i = 0; i < iFieldCount; ++i)
		{
			char szFieldName[128] = {0};
			strcpy_s(szFieldName, _countof(szFieldName), sqlite3_agent::sqlite3_column_name_fun(m_pSQLite3STMT, i));
			if (0 == strColumnName.CompareNoCase(szFieldName))
			{
				int iDataType = sqlite3_agent::sqlite3_column_type_fun(m_pSQLite3STMT, i);
				if (SQLITE_INTEGER == iDataType)
				{
					llValue = sqlite3_agent::sqlite3_column_int64_fun(m_pSQLite3STMT, i);

					bResult = TRUE;
				}
				break;
			}
		}
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			break;
		}
	}
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSQLite3::GetColumnValue
//
// 功能描述：获取列值 - 整型
//
// 输入参数：IN const CString& strColumnName	- 列名
//         ：IN OUT double& dValue				- 数据值
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2014年8月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::GetColumnValue(IN const CString& strColumnName, IN OUT double& dValue)
{
	BOOL bResult = FALSE;
	try
	{
		if (NULL == m_pSQLite3STMT)
			throw - 1;
		//获取指定表的字段数目
		int iFieldCount = sqlite3_agent::sqlite3_column_count_fun(m_pSQLite3STMT);
		//字段数目大于0,表示数据存在,开始解析读取
		if (0 == iFieldCount)
			throw - 2;
		for (int i = 0; i < iFieldCount; ++i)
		{
			char szFieldName[128] = {0};
			strcpy_s(szFieldName, _countof(szFieldName), sqlite3_agent::sqlite3_column_name_fun(m_pSQLite3STMT, i));
			if (0 == strColumnName.CompareNoCase(szFieldName))
			{
				int iDataType = sqlite3_agent::sqlite3_column_type_fun(m_pSQLite3STMT, i);
				if (SQLITE_FLOAT == iDataType)
				{
					dValue = sqlite3_agent::sqlite3_column_double_fun(m_pSQLite3STMT, i);

					bResult = TRUE;
				}
				break;
			}
		}
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			break;
		}
	}
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSQLite3::GetColumnValue
//
// 功能描述：获取列值 - 时间值
//
// 输入参数：IN const CString& strColumnName	- 列名
//         ：IN OUT CTime& cTime				- 数据值
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2014年8月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::GetColumnValue(IN const CString& strColumnName, IN OUT CTime& cTime)
{
	BOOL bResult = FALSE;
	try
	{
		if (NULL == m_pSQLite3STMT)
			throw - 1;
		//获取指定表的字段数目
		int iFieldCount = sqlite3_agent::sqlite3_column_count_fun(m_pSQLite3STMT);
		//字段数目大于0,表示数据存在,开始解析读取
		if (0 == iFieldCount)
			throw - 2;
		for (int i = 0; i < iFieldCount; ++i)
		{
			char szFieldName[128] = {0};
			strcpy_s(szFieldName, _countof(szFieldName), sqlite3_agent::sqlite3_column_name_fun(m_pSQLite3STMT, i));
			if (0 == strColumnName.CompareNoCase(szFieldName))
			{
				int iDataType = sqlite3_agent::sqlite3_column_type_fun(m_pSQLite3STMT, i);
				if (SQLITE_TEXT == iDataType)
				{
					CString strValue;
					strValue.Format(_T("%s"), sqlite3_agent::sqlite3_column_text_fun(m_pSQLite3STMT, i));
					COleDateTime oleGetTime;
					oleGetTime.ParseDateTime(strValue);

					COleDateTime t1 = (time_t)0;
					COleDateTime t2 = oleGetTime.m_dt;

					COleDateTimeSpan tSpan;
					tSpan = t2 - t1;

					CTime cTempTime(time_t(tSpan.GetTotalSeconds()));
					cTime = cTempTime;

					bResult = TRUE;
				}
				break;
			}
		}
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			break;
		}
	}
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSQLite3::AsciiToUnicode
//
// 功能描述：ASCII字符串转化成Unicode字符串
//
// 输入参数：IN const char* szString
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2014年8月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
const CString  CSQLite3::AsciiToUnicode(IN const char* szString)
{
	CString strString;
	int iAsciiLen = (int)strlen(szString);
	int  iUnicodeLen = ::MultiByteToWideChar( CP_ACP, 0, szString, -1, NULL, 0 );
	wchar_t *  pUnicode = NULL;
	pUnicode = new  wchar_t[iUnicodeLen + 1];
	memset(pUnicode, 0, (iUnicodeLen + 1)*sizeof(wchar_t));
	::MultiByteToWideChar( CP_ACP, 0, szString, -1, (LPWSTR)pUnicode, iUnicodeLen);
	strString.Format(_T("%s"), ( wchar_t* )pUnicode);
	delete  pUnicode;
	return  strString;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSQLite3::UnicodeToAscii
//
// 功能描述：Unicode字符串转化成ASCII字符串
//
// 输入参数：IN const CString& strString
// 输出参数：char*
// 编写人员：ROCY
// 编写时间：2014年8月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
char* CSQLite3::UnicodeToAscii(IN const CString& strString)
{
	/*
		int iLen = WideCharToMultiByte(CP_ACP,0,strString,strString.GetLength(),NULL,0,NULL,NULL);
		char* pszSQL = new char[iLen+1];
		if(NULL != pszSQL)
		{
			memset(pszSQL,0,iLen+1);
			WideCharToMultiByte(CP_ACP,0,strString,strString.GetLength() ,pszSQL,iLen + 1 ,NULL,NULL);
			return pszSQL;
		}
	*/
	int iLen = strString.GetLength();
	char* pszSQL = NULL;
	if (iLen > 0)
	{
		pszSQL = new char[iLen + 1];
		if (NULL != pszSQL)
		{
			memset(pszSQL, 0, iLen + 1);
			CString strTemp(strString);
			strcpy(pszSQL, strTemp.GetBuffer(0));
		}
	}
	return pszSQL;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSQLite3::AutoShrinkageDatabase
//
// 功能描述：设置自动收缩数据库
//
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2014年8月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::AutoShrinkageDatabase(void)
{
	BOOL bResult = FALSE;
	bResult = ExecuteSQL(_T("PRAGMA auto_vacuum = 1;"));
	return bResult;
}





