/*
 * @Description: 使用QSettings实现配置文件:读取储存器Qsetting
 * @Author: Ivy
 * @Date: 2022-05-09 09:34:43
 * @LastEditTime: 2022-05-09 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once


#pragma once
#include <QSettings>
class SDSetting
{
public:
	explicit SDSetting();
	~SDSetting();
public:
	QVariant readSetting(const QString& group, const QString& key, const QVariant& defaultValue);
	QVariant readSetting(const QString& key, const QVariant& defaultValue = QVariant());
	void writeSetting(const QString& group, const QString& key, const QVariant& value);
	void writeSetting(const QString& key, const QVariant& value);
	void removeSetting(const QString& group, const QString& key);
	void removeSetting(const QString& key);
	bool contains(const QString& group, const QString& key);
	bool contains(const QString& key);
	void setValue(const QString &key, const QVariant &value);
	QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;
private:
	QSettings* m_pSetting;
};
