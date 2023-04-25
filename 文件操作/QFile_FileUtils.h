/*
 * @Description: QT文件常用的函数
 * @Author: Ivy
 * @Date: 2022-04-24 09:34:43
 * @LastEditTime: 2023-02-24 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */


#pragma once
class QString;
 // 文件操作相关接口
namespace FileUtils
{
	// 计算md5的值
	QString md5(const QByteArray &data);

	// 获取指定路径文件md5值
	QString getFileMd5Value(const QString &filePath);

	// 移动文件到指定文件夹
	bool moveFile(const QString &filePath, const QString &targetPath);

	// 拷贝文件到指定文件夹
	bool copyFileToDir(const QString &filePath, const QString &targetDir);

	// 拷贝整个目录下的文件到指定文件夹下
	bool copyDirFileToDir(const QString &srcPath, const QString &dstPath, bool cover = false);

	// 读取文件数据
	bool readFileData(const QString &filePath, QByteArray &retData);

	// 重命名文件是否保留原来文件
	QString renameAndCalcMd5(const QString &oldPath, const QString &newPath, bool isKeepOldFile = false);
};

