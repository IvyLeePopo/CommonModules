/*
 * @Description: 数据库管理中心
				主要用了QSqlDatabase数据库管理数据，也有在本地保存当前的数据
 * @Author: Ivy
 * @Date: 2022-05-10 09:34:43
 * @LastEditTime: 2022-05-10 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once


#pragma once
#include <QReadWriteLock>
#include <QSqlDatabase>
#include <QObject>

#include "colorschemeitem.h"
#include "fileitem.h"
#include "material/materialitem.h"
#include "pattern/patternresitem.h"
#include "shoeitem.h"
#include "texture/textureitem.h"
#include "renderadditionaldata.h"

template <class T, class LockT = QMutex>
class SdData
{
public:
	SdData(){}

	SdData(const T &data)
	{
		m_data = data;
	}

	SdData& operator=(T &data)
	{
		m_data = data;
		return *this;
	}

	~SdData(){}

	T *operator->()
	{
		return &m_data;
	}
public:
	// 用于在数据库多线程操作中调用的锁，使用时要控制好使用的的范围，防止操作时间过长导致进程卡住
	LockT   m_lock;

private:
	T   m_data;
};

class DataManager : public QObject
{
	Q_OBJECT
public:
	// control对象用于在外面提供一个对象来控制超时控件的调用
	explicit DataManager(QObject *parent = nullptr);
	~DataManager();

public:
	bool init(const QString &userId, const QString &token);
	bool unInit();

	// 清除缓存
	void clearCache();

	void setUserInfo(const QString &userId, const QString &token);

	// 初始化数据有两个步骤，一个是通过数据库查询初始化，数据库查询完后再通过网络请求把数据再同步回来
	void initMaterialsData();
	void clearAllData();

	// 添加数据
	bool addMaterialData(std::shared_ptr<MaterialItem> materialItem);
	bool addColorSchemeData(std::shared_ptr<ColorSchemeItem> colorSchemeItem);

	// 更新数据
	bool updateMaterialData(std::shared_ptr<MaterialItem> item);
	bool updateColorSchemeData(std::shared_ptr<ColorSchemeItem> colorSchemeItem);

	// 获取数据
	bool getColorSchemeData(const QString &id, std::shared_ptr<ColorSchemeItem> *item = nullptr);
	// 通过已经有的材质获取一个新id的材质
	std::shared_ptr<MaterialItem> getNewTempMatFromExistMat(const QString &matId);
	QList<std::shared_ptr<MaterialItem>> getNotUploadMatList();


	// 这个方法只给从Datamanager中取出的数据指针时才使用的
	// 添加临时的材质包到内存中，不写到数据库里
	std::shared_ptr<MaterialItem> addTempMaterial(const QColor &color);

public:
	// 给外部调用的更新数据的接口,这个方法不用于保存过程状态，像uploading,downloading
	// 更新材质信息但不更新到数据库中
	static void updateMaterialItemNotToDb(std::shared_ptr<MaterialItem> item);

	// 下面更新的数据状态不会用来记录到数据库中，只在当前进程中有效
	static bool updateMaterialState(const QString &materialId,	SdFileState materialState);

private:
	bool            m_init;
	QString         m_userId;
	QString         m_token;//是一种用于验证身份和授权访问的令牌
	QString         m_dbPath;
	QSqlDatabase    m_sqlDB;
	QMutex          m_dbLock;
	SdData<QHash<QString, std::shared_ptr<MaterialItem>>, QReadWriteLock> m_mapMaterials;
	// 3d配色方案的子项
	SdData<QMap<QString, std::shared_ptr<ColorSchemeItem>>> m_mapColorSchemes;
};
