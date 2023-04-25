
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
		// MAX_PATH - 1��Ϊ���ٱ���һ��λ�ø��ַ���β���������������
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
		// ������ļ������ļ����ڵĻ���˵������ļ��ڱ����Ѿ������ˣ����Ծ��Ȳ�������
		if (!QFile::exists(newPath))
		{
			// Ҫ�����ļ���λ�ã�����ļ�λ�ò��ǻ���λ�õģ�˵������ļ����û�ͨ�����ļ��Ի���ѡ��򿪵ģ�
			// ��ʱ�Ͳ���ֱ�Ӱ��ļ��ƶ�������Ŀ¼�У�Ҫ���Ƶ�����Ŀ¼��
			if (!PathUtils::isSdCacheFilePath(oldPath))
			{
				copyFileToDir(oldPath, PathUtils::getSdFileCachePath());
				const QString oldCachePath = PathUtils::getSdFullPathByName(QFileInfo(oldPath).fileName());
				// ������ʧ�ܾͲ����ϴ�����Ϊ���ܻᵼ�°��ƶ˵��ļ������ǵ�
				if (!isKeepOldFile)//����������Ŀ¼�²������ļ�
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
				// ������ʧ�ܾͲ����ϴ�����Ϊ���ܻᵼ�°��ƶ˵��ļ������ǵ�
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
		// ������ļ��Ѿ����ڵģ��Ϳ����þɵ���������ȡmd5ֵ
		else
		{
			std::shared_ptr<FileItem> fileItem;
			// ����ļ��ڵ���ڣ�����md5ֵ���ڣ���ֱ��ȡ���ݿ��е�ֵ
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