/*
 * @Description: QT文件常用的函数
 * @Author: Ivy
 * @Date: 2022-04-24 09:34:43
 * @LastEditTime: 2023-02-24 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */


#pragma once

#define Q_OS_WIN

class QString;
 // 文件操作相关接口
namespace FileUtils
{
	// 1、计算md5的值
	QString md5(const QByteArray &data);

	// 2、获取指定路径文件md5值
	QString getFileMd5Value(const QString &filePath);

	// 3、移动文件到指定文件夹
	bool moveFile(const QString &filePath, const QString &targetPath);

	// 4、拷贝文件到指定文件夹
	bool copyFileToDir(const QString &filePath, const QString &targetDir);

	// 5、拷贝整个目录下的文件到指定文件夹下
	bool copyDirFileToDir(const QString &srcPath, const QString &dstPath, bool cover = false);

	// 6、读取文件数据
	bool readFileData(const QString &filePath, QByteArray &retData);

	// 7、重命名文件是否保留原来文件
	QString renameAndCalcMd5(const QString &oldPath, const QString &newPath, bool isKeepOldFile = false);

	// 8、获取指定所有的指定文件
	bool getDirAllFiles(string strSuffix, string strDir, vector<tagFileInfo>& vecFiles);
};

