#include "QSettingsConfig.h"
#include <QDebug>
#include <QTextCodec>
#include <QTime>

QSettingsConfig *QSettingsConfig::self = nullptr;

QSettingsConfig *QSettingsConfig::instance()
{
    if (self == nullptr) {
        self = new QSettingsConfig();
    }
    return self;
}

QSettingsConfig::QSettingsConfig(QObject *parent) : QObject(parent)
{
    m_strProvinceName = "";
    m_nStartMsec = 0;
    m_nEndMsec = 0;
}

QSettingsConfig::~QSettingsConfig()
{
}

QString QSettingsConfig::getConfigValue(QString strGroup, QString strKey, QString strDefaultValue)
{
    m_mutex.tryLock(500);

    QSettings settings(INI_PATH, QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("UTF-8"));

    settings.beginGroup(strGroup);
    if (settings.value(strKey).isNull()) {
        qDebug() << strGroup << "-" << strKey << "不存在，创建默认值";
        settings.setValue(strKey, strDefaultValue);
        settings.sync();
    }
    QVariant var = settings.value(strKey);
    settings.endGroup();

    m_mutex.unlock();

    return var.toString();
}

int QSettingsConfig::getConfigValue(QString strGroup, QString strKey, int nDefaultValue)
{
    m_mutex.tryLock(500);

    QSettings settings(INI_PATH, QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("UTF-8"));

    settings.beginGroup(strGroup);
    if (settings.value(strKey).isNull()) {
        qDebug() << strGroup << "-" << strKey << "不存在，创建默认值";
        settings.setValue(strKey, nDefaultValue);
        settings.sync();
    }
    QVariant var = settings.value(strKey);
    settings.endGroup();

    m_mutex.unlock();

    return var.toInt();
}

double QSettingsConfig::getConfigValue(QString strGroup, QString strKey, double fDefaultValue)
{
    m_mutex.tryLock(500);

    QSettings settings(INI_PATH, QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("UTF-8"));

    settings.beginGroup(strGroup);
    if (settings.value(strKey).isNull()) {
        qDebug() << strGroup << "-" << strKey << "不存在，创建默认值";
        settings.setValue(strKey, fDefaultValue);
        settings.sync();
    }
    QVariant var = settings.value(strKey);
    settings.endGroup();

    m_mutex.unlock();

    return var.toDouble();
}

QString QSettingsConfig::getProvinceNameFromConfig()
{
    if (m_strProvinceName.isEmpty()) {
        QString strLaneId = SETTINGS("WebSocket", "VideoCaptureURL", "6200000000").right(10);
        switch (strLaneId.left(2).toInt()) {
        case 12: m_strProvinceName = "天津"; break;
        case 41: m_strProvinceName = "河南"; break;
        case 43: m_strProvinceName = "湖南"; break;
        case 62: m_strProvinceName = "甘肃"; break;
        case 63: m_strProvinceName = "青海"; break;
        default: m_strProvinceName = "甘肃"; break;
        }
        qDebug() << "从VideoCaptureURL获取AppID:" << strLaneId << "，对应省份: " << m_strProvinceName;
    }
    return m_strProvinceName;
}

void QSettingsConfig::setConfigValue(QString strGroup, QString strKey, QString strValue)
{
    m_mutex.tryLock(500);

    QSettings settings(INI_PATH, QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("UTF-8"));

    settings.beginGroup(strGroup);
    settings.setValue(strKey, strValue);
    settings.sync();
    settings.endGroup();

    m_mutex.unlock();

    return;
}

void QSettingsConfig::setConfigValue(QString strGroup, QString strKey, int nValue)
{
    m_mutex.tryLock(500);

    QSettings settings(INI_PATH, QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("UTF-8"));

    settings.beginGroup(strGroup);
    settings.setValue(strKey, nValue);
    settings.sync();
    settings.endGroup();

    m_mutex.unlock();

    return;
}

void QSettingsConfig::setConfigValue(QString strGroup, QString strKey, double fValue)
{
    m_mutex.tryLock(500);

    QSettings settings(INI_PATH, QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("UTF-8"));

    settings.beginGroup(strGroup);
    settings.setValue(strKey, fValue);
    settings.sync();
    settings.endGroup();

    m_mutex.unlock();

    return;
}

bool QSettingsConfig::isDayLight()
{
    if (m_nStartMsec == 0 && m_nEndMsec == 0) {
        m_nStartMsec = SETTINGS("LightOption", "startHour", 7) * 3600 * 1000;
        m_nEndMsec = SETTINGS("LightOption", "endHour", 18) * 3600 * 1000;
    }

    int nNowMsec = QTime::currentTime().msecsSinceStartOfDay();
    if (nNowMsec > m_nStartMsec && nNowMsec < m_nEndMsec) {
        qDebug() << "当前时间为白天";
        return true;
    }
    else {
        qDebug() << "当前时间为夜晚";
        return false;
    }
}
