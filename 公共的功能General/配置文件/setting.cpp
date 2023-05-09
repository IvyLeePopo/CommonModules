#include "pch.h"
#include "setting.h"
#include "defs.h"
SDSetting::SDSetting()
{
#ifdef Q_OS_WIN
	m_pSetting = new QSettings(REG_SHOETECH, QSettings::NativeFormat);
#else
	m_pSetting = new QSettings(REG_SHOETECH, QSettings::IniFormat);
#endif
}

SDSetting::~SDSetting()
{
	if (m_pSetting)
		delete m_pSetting;
}

QVariant SDSetting::readSetting(const QString& group, const QString& key, const QVariant& defaultValue)
{
	m_pSetting->beginGroup(group);
	QVariant variantValue = m_pSetting->value(key, defaultValue);
	m_pSetting->endGroup();
	return variantValue;
}

QVariant SDSetting::readSetting(const QString& key, const QVariant& defaultValue /*= QVariant()*/)
{
	return m_pSetting->value(key, defaultValue);
}

void SDSetting::writeSetting(const QString& group, const QString& key, const QVariant& value)
{
	m_pSetting->beginGroup(group);
	m_pSetting->setValue(key, value);
	m_pSetting->endGroup();
}

void SDSetting::writeSetting(const QString& key, const QVariant& value)
{
	m_pSetting->setValue(key, value);
}

void SDSetting::removeSetting(const QString& group, const QString& key)
{
	m_pSetting->beginGroup(group);
	m_pSetting->remove(key);
	m_pSetting->endGroup();
}

void SDSetting::removeSetting(const QString& key)
{
	m_pSetting->remove(key);
}

bool SDSetting::contains(const QString& group, const QString& key)
{
	m_pSetting->beginGroup(group);
	bool bContain = m_pSetting->contains(key);
	m_pSetting->endGroup();
	return bContain;
}

bool SDSetting::contains(const QString& key)
{
	return m_pSetting->contains(key);
}

void SDSetting::setValue(const QString &key, const QVariant &value)
{
	m_pSetting->setValue(key, value);
}

QVariant SDSetting::value(const QString &key, const QVariant &defaultValue /*= QVariant()*/) const
{
	return m_pSetting->value(key, defaultValue);
}
