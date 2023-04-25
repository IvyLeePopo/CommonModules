
#include "fileutils.h"
#include <QDir>
#include <QString>
#include <QFileInfo>
#include "application/pubglobalinstances.h"
#include "datamanager/datamanager.h"
#include "datamanager/fileitem.h"
#include "pathutils.h"
#ifdef Q_OS_WIN
#include <Windows.h>
#include <shellapi.h>
#endif

namespace FileUtils
{
	QString md5(const QByteArray &data)
	{
		QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Md5);
		return hash.toHex();
	}

	QString getFileMd5Value(const QString &filePath)
	{
		if (QFileInfo(filePath).isDir())
		{
			LOG(INFO) << "path is direct: " << filePath.toLatin1().data();
			return "";
		}
		QString md5Val;
		QFile file(filePath);
		if (!file.open(QIODevice::ReadOnly))
		{
			return md5Val;
		}
		QByteArray dataArray = file.readAll();
		md5Val = md5(dataArray);
		file.close();
		return md5Val;
	}

	bool moveFile(const QString &filePath, const QString &targetPath)
	{
#ifdef Q_OS_WIN
		WCHAR szFrom[MAX_PATH] = {};
		memset(szFrom, 0, sizeof(szFrom));
		// MAX_PATH - 1是为了再保留一个位置给字符串尾部添加两个结束符
		wcscpy_s(szFrom, MAX_PATH - 1, (wchar_t *)QDir::toNativeSeparators(filePath).utf16());
		szFrom[filePath.length() + 1] = 0;
		WCHAR szTo[MAX_PATH] = {};
		memset(szTo, 0, sizeof(szTo));
		wcscpy_s(szTo, MAX_PATH - 1, (wchar_t *)QDir::toNativeSeparators(targetPath).utf16());
		szTo[targetPath.length() + 1] = 0;
		SHFILEOPSTRUCTW shFile = {};
		shFile.wFunc = FO_MOVE;
		shFile.fFlags = FOF_NO_UI;
		shFile.pFrom = szFrom;
		shFile.pTo = szTo;
		return (bool)!SHFileOperationW(&shFile);
#else
		QString newPath = jointPath(targetPath, QFileInfo(filePath).fileName());
		return QFile::rename(filePath, newPath);
#endif
	}

	bool copyFileToDir(const QString &filePath, const QString &targetDir)
	{
		if (targetDir.isEmpty() || !QFile::exists(filePath))
			return false;
		if (!QDir(targetDir).exists())
		{
			const bool bCreate = QDir().mkpath(targetDir);
			if (!bCreate)
				return false;
		}
		const QString newPath = PathUtils::jointPath(targetDir, QFileInfo(filePath).fileName());
		const bool b = QFile::copy(filePath, newPath);
		return b;
	}

	bool readFileData(const QString &filePath, QByteArray &retData)
	{
		QFile file(filePath);
		if (!file.open(QFile::ReadOnly))
		{
			return false;
		}
		retData = file.readAll();
		file.close();
		return true;
	}

	QString renameAndCalcMd5(const QString &oldPath, const QString &newPath, bool isKeepOldFile)
	{
		QString newMd5;
		if (QFileInfo(oldPath).isDir())
		{
			return newMd5;
		}
		// 如果新文件名的文件存在的话，说明这个文件在本地已经存有了，所以就先不重命名
		if (!QFile::exists(newPath))
		{
			// 要区分文件的位置，如果文件位置不是缓存位置的，说明这个文件是用户通过打开文件对话框选择打开的，
			// 这时就不能直接把文件移动到缓存目录中，要复制到缓存目录中
			if (!PathUtils::isSdCacheFilePath(oldPath))
			{
				copyFileToDir(oldPath, PathUtils::getSdFileCachePath());
				const QString oldCachePath = PathUtils::getSdFullPathByName(QFileInfo(oldPath).fileName());
				// 重命名失败就不用上传，因为可能会导致把云端的文件给覆盖掉
				if (!isKeepOldFile)//不保留缓存目录下操作的文件
				{
					if (!QFile::rename(oldCachePath, newPath))
					{
						LOG(INFO) << "rename fail :oldpath=" << oldPath.toLatin1().data() << " ,newpath=" << newPath.toLatin1().data();
						return newMd5;
					}
				}
				else
				{
					if (!QFile::copy(oldCachePath, newPath))
					{
						LOG(INFO) << "rename fail :oldpath=" << oldPath.toLatin1().data() << " ,newpath=" << newPath.toLatin1().data();
						return newMd5;
					}
				}
			}
			else
			{
				// 重命名失败就不用上传，因为可能会导致把云端的文件给覆盖掉
				if (!isKeepOldFile)
				{
					if (!QFile::rename(oldPath, newPath))
					{
						LOG(INFO) << "rename fail :oldpath=" << oldPath.toLatin1().data() << " ,newpath=" << newPath.toLatin1().data();
						return newMd5;
					}
				}
				else
				{
					if (!QFile::copy(oldPath, newPath))
					{
						LOG(INFO) << "rename fail :oldpath=" << oldPath.toLatin1().data() << " ,newpath=" << newPath.toLatin1().data();
						return newMd5;
					}
				}
			}
			newMd5 = PathUtils::transformShoePicNameWithoutExt(QString(), newPath);
			//newMd5 = getFileMd5Value(newPath);
		}
		// 如果新文件已经存在的，就可以用旧的数据来获取md5值
		else
		{
			std::shared_ptr<FileItem> fileItem;
			// 如果文件节点存在，而且md5值存在，就直接取数据库中的值
			if (DataManagerInstance
				&& DataManagerInstance->getFileData(QFileInfo(newPath).fileName(), &fileItem)
				&& !fileItem->getMD5().isEmpty())
			{
				newMd5 = fileItem->getMD5();
			}
			else
			{
				newMd5 = PathUtils::transformShoePicNameWithoutExt(QString(), newPath);
				//newMd5 = getFileMd5Value(newPath);
			}
		}
		return newMd5;
	}

	bool copyDirFileToDir(const QString &srcPath, const QString &dstPath, bool cover/* = false*/)
	{
		QDir srcDir(srcPath);
		QDir dstDir(dstPath);
		if (!dstDir.exists())
		{
			if (!dstDir.mkdir(dstDir.absolutePath()))
				return false;
		}
		QFileInfoList fileInfoList = srcDir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
		for (const QFileInfo &fileInfo : fileInfoList)
		{
			QString dstPathNew = dstDir.filePath(fileInfo.fileName());
			if (fileInfo.isDir())
			{
				if (!copyDirFileToDir(fileInfo.filePath(), dstPathNew, cover))
					return false;
			}
			else
			{
				if (dstDir.exists(fileInfo.fileName()))
				{
					if (cover)
						dstDir.remove(fileInfo.fileName());
					else
						continue;
				}
				if (!QFile::copy(fileInfo.filePath(), dstPathNew))
					return false;
			}
		}
		return true;
	}
}