/*
 * @Description: C文件的常见用法
 * @Author: Ivy
 * @Date: 2022-05-10 09:34:43
 * @LastEditTime: 2022-05-10 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once
struct tagFileInfo
{
	ULONG uLen;
	int nCreateTime;
	int nModifyTime;
	string strFileName;
};

 // 1、是否存在文件或者目录
bool isExistFile(string strFile, bool bIsDir = false);

// 2、获取指定目录下的所有指定格式文件,返回的列表将按照创建时间排序，最早的在最前面
bool getDirFiles(string strDir, vector<tagFileInfo>& vecFiles, string strSuffixName);

// 3、保存文件内容
/*
	strMode词典如下:
	r	以只读方式打开文件，该文件必须存在。
	r+	以读/写方式打开文件，该文件必须存在。
	rb+	以读/写方式打开一个二进制文件，只允许读/写数据。
	rt+	以读/写方式打开一个文本文件，允许读和写。
	w	打开只写文件，若文件存在则文件长度清为零，即该文件内容会消失；若文件不存在则创建该文件。
	w+	打开可读/写文件，若文件存在则文件长度清为零，即该文件内容会消失；若文件不存在则创建该文件。
	a	以附加的方式打开只写文件。若文件不存在，则会创建该文件；如果文件存在，则写入的数据会被加到文件尾后，即文件原先的内容会被保留（EOF 符保留）。
	a+	以附加方式打开可读/写的文件。若文件不存在，则会创建该文件，如果文件存在，则写入的数据会被加到文件尾后，即文件原先的内容会被保留（EOF符不保留）。
	wb	以只写方式打开或新建一个二进制文件，只允许写数据。
	wb+	以读/写方式打开或新建一个二进制文件，允许读和写。
	wt+	以读/写方式打开或新建一个文本文件，允许读和写。
	at+	以读/写方式打开一个文本文件，允许读或在文本末追加数据。
	ab+	以读/写方式打开一个二进制文件，允许读或在文件末追加数据。
*/
bool setFileContent(string strFile, string& strContent, string strMode = "w+");

// 4、获取文件内容
bool getFileContent(string strFile, string& strContent, bool bHex = false);

// 5、保存二进制文件内容
bool setFileBinary(string strFile, const char* pszData, int nSize);

// 6、获取文件内容，二进制的方式,谨记，用过本函数需要释放pszData！！！！！！！！
bool getFileBinary(string strFile, char** pszData, int& nSize);

// 7、新建文件或者文件夹
bool createNewFileOrDir(string strFile, bool bIsDir = false);

// 8、获取文件的长度
long getFileSize(const char* pszFileName);

// 9、删除文件
bool deleteFile(const char* pszFileName);

// 10、write to file
bool write2File(string strFilePath, string& strContent);

// 11、read file
bool readFile(string strFilePath, string& strContent);

namespace linux_file
{
	// 1、判断文件或者目录是否存在,strFileName最好是全路径的文件
	bool isExistFile(string strFileName);

	// 2、创建目录
	bool createDir(string strDir);
}
