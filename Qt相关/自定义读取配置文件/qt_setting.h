#pragma once

#include <QSettings>

class QtSetting
{
public:
	explicit QtSetting();
	~QtSetting();

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
