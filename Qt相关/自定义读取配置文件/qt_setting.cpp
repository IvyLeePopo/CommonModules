#include "pch.h"
#include "qt_setting.h"

#include "defs.h"

QtSetting::QtSetting()
{
#ifdef Q_WS_WIN
	m_pSetting = new QSettings(REG_SHOETECH, QSettings::NativeFormat);
#else
	m_pSetting = new QSettings(REG_SHOETECH, QSettings::IniFormat);
#endif
}

QtSetting::~QtSetting()
{
	if (m_pSetting)
		delete m_pSetting;
}

QVariant QtSetting::readSetting(const QString& group, const QString& key, const QVariant& defaultValue)
{
	m_pSetting->beginGroup(group);
	QVariant variantValue = m_pSetting->value(key, defaultValue);
	m_pSetting->endGroup();

	return variantValue;
}

QVariant QtSetting::readSetting(const QString& key, const QVariant& defaultValue /*= QVariant()*/)
{
	return m_pSetting->value(key, defaultValue);
}

void QtSetting::writeSetting(const QString& group, const QString& key, const QVariant& value)
{
	m_pSetting->beginGroup(group);
	m_pSetting->setValue(key, value);
	m_pSetting->endGroup();
}

void QtSetting::writeSetting(const QString& key, const QVariant& value)
{
	m_pSetting->setValue(key, value);
}

void QtSetting::removeSetting(const QString& group, const QString& key)
{
	m_pSetting->beginGroup(group);
	m_pSetting->remove(key);
	m_pSetting->endGroup();
}

void QtSetting::removeSetting(const QString& key)
{
	m_pSetting->remove(key);
}

bool QtSetting::contains(const QString& group, const QString& key)
{
	m_pSetting->beginGroup(group);
	bool bContain = m_pSetting->contains(key);
	m_pSetting->endGroup();

	return bContain;
}

bool QtSetting::contains(const QString& key)
{
	return m_pSetting->contains(key);
}

void QtSetting::setValue(const QString &key, const QVariant &value)
{
	m_pSetting->setValue(key, value);
}

QVariant QtSetting::value(const QString &key, const QVariant &defaultValue /*= QVariant()*/) const
{
	return m_pSetting->value(key, defaultValue);
}
