#include "json.h"

bool isJsonIllegal(const char *pszJson)
{
	std::stack<char> jsonstr;
	char startBuf = pszJson[0];
	char endBuf = '\0';
	bool bIsObject = false; //防止 {}{}的判断
	bool bIsArray = false;  //防止[][]的判断

	while (*pszJson != '\0')
	{
		endBuf = *pszJson;
		switch (*pszJson)
		{
		case '{':
			if (!bIsObject)
			{
				bIsObject = true;
			}
			else  if (jsonstr.empty())//对象重复入栈
			{
				return false;
			}
			jsonstr.push('{');
			break;
		case '"':
			if (jsonstr.empty() || jsonstr.top() != '"')
			{
				jsonstr.push(*pszJson);
			}
			else
			{
				jsonstr.pop();
			}
			break;
		case '[':
			if (!bIsArray)
			{
				bIsArray = true;
			}
			else  if (jsonstr.empty())//数组重复入栈
			{
				return false;
			}
			jsonstr.push('[');
			break;
		case ']':
			if (jsonstr.empty() || jsonstr.top() != '[')
			{
				return false;
			}
			else
			{
				jsonstr.pop();
			}
			break;
		case '}':
			if (jsonstr.empty() || jsonstr.top() != '{')
			{
				return false;
			}
			else
			{
				jsonstr.pop();
			}
			break;
		case '\\'://被转义的字符,跳过
			pszJson++;
			break;
		default:
			;
		}
		pszJson++;
	}

	if (jsonstr.empty())
	{
		//确保是对象或者是数组,之外的都不算有效
		switch (startBuf)//确保首尾符号对应
		{
		case  '{':
		{

			if (endBuf = '}')
			{
				return true;
			}
			return false;
		}
		case '[':
		{
			if (endBuf = ']')
			{
				return true;
			}
			return false;
		}
		default:
			return false;
		}

		return true;
	}

	return false;
}