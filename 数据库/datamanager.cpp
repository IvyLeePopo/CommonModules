#include "pch.h"
#include "datamanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QFileInfo>
#include "application/pubglobalinstances.h"
#include "common/global.h"
#include "datathread.h"
#include "general/genericfunc.h"
#include "general/jsonutils.h"
#include "general/pathutils.h"
#define  DB_NAME        "sd.dat"

DataManager::DataManager(QObject *parent)
	: QObject(parent)
	, m_init(false)
{

}
DataManager::~DataManager()
{
	unInit();
}
bool DataManager::init(const QString &userId, const QString &token)
{
	if (m_init)
	{
		return false;
	}
	m_userId = userId;
	m_token = token;
	m_dbPath = QString("%1/%2/%3").arg(PathUtils::getSdAppDataPath()).arg(userId).arg(DB_NAME);
	m_sqlDB = QSqlDatabase::addDatabase("QSQLITE");
	m_sqlDB.setDatabaseName(m_dbPath);     //如果本目录下没有该文件,则会在本目录下生成,否则连接该文件
	if (!m_sqlDB.open())
	{
		return false;
	}

	initMaterialsData();

	m_init = true;
	return true;
}
bool DataManager::unInit()
{
	if (!m_init)
	{
		return false;
	}
	m_sqlDB.close();
	m_init = false;
	return true;
}

void DataManager::clearCache()
{
	//>%appdata%/ideation
	QString cachePath = PathUtils::getSdFileCachePath();
	QString dataBasePath = PathUtils::getSdUserAppDataPath();
	dataBasePath.append("/sd.dat");
	//>%temp%/shoetechtmp
	QString sdTempPath = PathUtils::getSdTempPath();
	QDir cacheDir(cachePath);
	QDir tempDir(sdTempPath);
	QFileInfoList totalFileList;
	cacheDir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //filter
	totalFileList = cacheDir.entryInfoList();
	tempDir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //filter
	totalFileList.append(tempDir.entryInfoList());
	foreach(QFileInfo file, totalFileList)
	{
		if (file.isFile())
		{
			file.dir().remove(file.fileName());
		}
		else
		{
			global::deleteDir(file.absoluteFilePath());
		}
	}
	//>先清除内存数据，关闭数据库，再删除数据库文件，最后在重新打开数据库；
	clearAllData();
	unInit();
	QFileInfo file(dataBasePath);
	file.dir().remove(file.fileName());
	init(SdLoginManagerInstance->getUserId(), SdLoginManagerInstance->getToken());
}

void DataManager::setUserInfo(const QString &userId, const QString &token)
{
	m_userId = userId;
	m_token = token;
}

void DataManager::initMaterialsData()
{
	QSqlQuery query(m_sqlDB);
	QString sqlString = "create table if not exists materials ("
		"id varchar(30) primary key not null,"
		"name varchar(30), "
		"thumbnail varchar(256), "
		"flatthumbnail varchar(256), "
		"state int, "
		"packagepath varchar(256), "
		"md5 varchar(16), "
		"details varchar(0), "
		"extend varchar(256)"
		")";
	query.prepare(sqlString);
	if (!query.exec())
	{
		LOG(INFO) << "Error: Fail to create table: " << query.lastError().text().toStdString().c_str();
		return;
	}
	else
	{
		LOG(INFO) << "Table created!";
	}
	QString selectSql = "select * from materials";
	DataThread *dataThread = new DataThread(m_dbPath, selectSql, [=](QSqlQuery *query)
	{
		QHash<QString, std::shared_ptr<MaterialItem>> mapMaterials;
		while (query->next())
		{
			std::shared_ptr<MaterialItem>materialsItem(new MaterialItem);
			materialsItem->setId(query->value(0).toString());
			materialsItem->setName(query->value(1).toString());
			materialsItem->setThumbnail(query->value(2).toString());
			materialsItem->setFlatThumbnail(query->value(3).toString());
			materialsItem->setState((SdFileState)query->value(4).toInt());
			materialsItem->setPackagePath(query->value(5).toString());
			materialsItem->setMD5(query->value(6).toString());
			mapMaterials.insert(materialsItem->getId(), materialsItem);
		}
		QWriteLocker lock(&m_mapMaterials.m_lock);
		m_mapMaterials = mapMaterials;
	});
	dataThread->start();
}

void DataManager::clearAllData()
{
	m_userId = "";
	m_token = "";
	{
		QMutexLocker lock(&m_mapShoes.m_lock);
		m_mapShoes->clear();
	}
	{
		QWriteLocker lock(&m_mapMaterials.m_lock);
		m_mapMaterials->clear();
	}
	{
		QMutexLocker lock(&m_mapFiles.m_lock);
		m_mapFiles->clear();
	}
}

bool DataManager::addMaterialData(std::shared_ptr<MaterialItem> materialItem)
{
	{
		QWriteLocker lock(&m_mapMaterials.m_lock);
		if (m_mapMaterials->find(materialItem->getId()) != m_mapMaterials->end())
		{
			return false;
		}
		m_mapMaterials->insert(materialItem->getId(), materialItem);
	}
	// 当前不保存项目的过程状态，只保存下载和未下载状态
	SdFileState state = materialItem->getState();
	if (state != SFS_Normal && state != SFS_NotDownload && state != SFS_NotUpload)
	{
		state = SFS_NotDownload;
	}
	if (!m_init)
		return false;
	QMutexLocker lock(&m_dbLock);
	QSqlQuery query(m_sqlDB);
	QString sqlString = QString("insert into materials "
		"(id, name, thumbnail, flatthumbnail, state, packagepath, md5) "
		"values ('%1', '%2', '%3', '%4', %5, '%6', '%7')").\
		arg(materialItem->getId()).\
		arg(materialItem->getName()).\
		arg(materialItem->getThumbnail()).\
		arg(materialItem->getFlatThumbnail()).\
		arg(state).\
		arg(materialItem->getPackagePath()).\
		arg(materialItem->getMD5());
	if (!query.exec(sqlString))
	{
		LOG(INFO) << "insert materials item fail: " << query.lastError().text().toStdString().c_str();
		return false;
	}
	else
	{
		LOG(INFO) << "insert materials item success!";
	}
	return true;
}

bool DataManager::addColorSchemeData(std::shared_ptr<ColorSchemeItem> colorSchemeItem)
{
	if (colorSchemeItem->getId().isEmpty())
		return false;
	{
		QMutexLocker lock(&m_mapColorSchemes.m_lock);
		if (m_mapColorSchemes->find(colorSchemeItem->getId()) != m_mapColorSchemes->end())
		{
			return false;
		}
		m_mapColorSchemes->insert(colorSchemeItem->getId(), colorSchemeItem);
	}
	return true;
}

bool DataManager::updateMaterialData(std::shared_ptr<MaterialItem> item)
{
	// 当前不保存项目的过程状态，只保存下载和未下载状态
	SdFileState state = item->getState();
	if (state != SFS_Normal && state != SFS_NotDownload)
	{
		return false;
	}
	QMutexLocker lock(&m_dbLock);
	QSqlQuery query(m_sqlDB);
	QString sqlString = QString("update materials set "
		"name = '%1', thumbnail = '%2', flatthumbnail = '%3', state = %4, packagepath = '%5', md5 = '%6', details = '%7'"
		"where id = '%8'").\
		arg(item->getName()).\
		arg(item->getThumbnail()).\
		arg(item->getFlatThumbnail()).\
		arg(item->getState()).\
		arg(item->getPackagePath()).\
		arg(item->getMD5()).\
		arg("").\
		arg(item->getId());
	if (!query.exec(sqlString))
	{
		LOG(INFO) << "update materials item fail: " << query.lastError().text().toStdString().c_str();
		return false;
	}
	else
	{
		LOG(INFO) << "update materials item success!";
	}
	return true;
}

bool DataManager::updateColorSchemeData(std::shared_ptr<ColorSchemeItem> colorSchemeItem)
{
	if (colorSchemeItem->getId().isEmpty())
		return false;
	{
		QMutexLocker lock(&m_mapColorSchemes.m_lock);
		if (m_mapColorSchemes->find(colorSchemeItem->getId()) == m_mapColorSchemes->end())
		{
			m_mapColorSchemes->insert(colorSchemeItem->getId(), colorSchemeItem);
		}
		else
		{
			m_mapColorSchemes->remove(colorSchemeItem->getId());
			m_mapColorSchemes->insert(colorSchemeItem->getId(), colorSchemeItem);
		}
	}
	return true;
}

bool DataManager::getColorSchemeData(const QString &id, std::shared_ptr<ColorSchemeItem> *item)
{
	if (id.isEmpty())
		return false;
	QMutexLocker lock(&m_mapColorSchemes.m_lock);
	QMap<QString, std::shared_ptr<ColorSchemeItem>>::iterator iter = m_mapColorSchemes->find(id);
	if (iter != m_mapColorSchemes->end())
	{
		if (item)
		{
			*item = iter.value();
		}
		return true;
	}
	return false;
}

std::shared_ptr <MaterialItem> DataManager::getNewTempMatFromExistMat(const QString &matId)
{
	std::shared_ptr<MaterialItem> existItem;
	if (!getMaterialData(matId, &existItem))
	{
		return nullptr;
	}
	std::shared_ptr<MaterialItem>materialItem(new MaterialItem);
	*materialItem.get() = *existItem.get();
	materialItem->setId(QString("%0%1").arg(TEMP_MAT_ID_PREFIX).arg(QUuid::createUuid().toString(QUuid::WithoutBraces)));
	materialItem->setState(SFS_NotUpload);
	materialItem->setStandarState(false);
	if (!MaterialItem::isTempMaterial(matId))
	{
		materialItem->setSrcMatId(matId);
	}
	else
	{
		materialItem->setSrcMatId(existItem->getSrcMatId());
	}
	{
		QWriteLocker lock(&m_mapMaterials.m_lock);
		m_mapMaterials->insert(materialItem->getId(), materialItem);
	}
	return materialItem;
}

QList<std::shared_ptr<MaterialItem>> DataManager::getNotUploadMatList()
{
	QList<std::shared_ptr<MaterialItem>> listMat;
	QReadLocker lock(&m_mapMaterials.m_lock);
	for (auto iter = m_mapMaterials->begin(); iter != m_mapMaterials->end(); ++iter)
	{
		std::shared_ptr<MaterialItem> item(iter.value());
		if (item->getState() == SFS_NotUpload)
		{
			listMat.push_back(item);
		}
	}
	return listMat;
}

std::shared_ptr <MaterialItem> DataManager::addTempMaterial(const QColor &color)
{
	Q_UNUSED(color);
	std::shared_ptr<MaterialItem>materialItem(new MaterialItem);
	materialItem->setId(QString("%0%1").arg(TEMP_MAT_ID_PREFIX).arg(QUuid::createUuid().toString(QUuid::WithoutBraces)));
	materialItem->setState(SFS_NotUpload);
	materialItem->setStandarState(false);
	{
		QWriteLocker lock(&m_mapMaterials.m_lock);
		m_mapMaterials->insert(materialItem->getId(), materialItem);
	}
	return materialItem;
}

void DataManager::updateMaterialItem(std::shared_ptr<MaterialItem> item)
{
	DataManager *dataMgr = DataManagerInstance;
	if (!dataMgr)
		return;
	if (!item)
		return;
	std::shared_ptr<MaterialItem> dataItem;
	if (!dataMgr->getMaterialData(item->getId(), &dataItem))
	{
		dataMgr->addMaterialData(item);
	}
	else
	{
		if (dataItem != item)
		{
			*dataItem = *item;
		}
		dataMgr->updateMaterialData(dataItem);
	}
}

void DataManager::updateMaterialItem(const MaterialItem& item)
{
	std::shared_ptr<MaterialItem> matItem(new MaterialItem(item));
	updateMaterialItem(matItem);
}

void DataManager::updateMaterialItemNotToDb(std::shared_ptr<MaterialItem> item)
{
	DataManager *dataMgr = DataManagerInstance;
	if (!dataMgr)
		return;
	if (!item)
		return;
	std::shared_ptr<MaterialItem> dataItem;
	if (!dataMgr->getMaterialData(item->getId(), &dataItem))
	{
		dataMgr->addMaterialDataNotToDb(item);
	}
	else
	{
		if (dataItem != item)
		{
			*dataItem = *item;
		}
	}
}

bool DataManager::updateMaterialState(const QString &materialId, SdFileState materialState)
{
	DataManager *dataMgr = DataManagerInstance;
	if (!dataMgr)
		return false;
	std::shared_ptr<MaterialItem>materialItem;
	if (dataMgr->getMaterialData(materialId, &materialItem))
	{
		materialItem->setState(materialState);
		dataMgr->updateMaterialData(materialItem);
		return true;
	}
	return false;
}
