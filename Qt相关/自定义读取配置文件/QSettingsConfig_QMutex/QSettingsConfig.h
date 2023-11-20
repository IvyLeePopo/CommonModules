#ifndef QSETTINGSCONFIG_H
#define QSETTINGSCONFIG_H

#include <QObject>
#include <QSettings>
#include <QMutex>

#define INI_PATH "/home/apps/TwBoothService/GlobalConfig.ini"
#define SETTINGS QSettingsConfig::instance()->getConfigValue
#define PROVINCE QSettingsConfig::instance()->getProvinceNameFromConfig()
#define SETVALUE QSettingsConfig::instance()->setConfigValue

class QSettingsConfig : public QObject
{
    Q_OBJECT
public:
    static QSettingsConfig *instance();
    QString getConfigValue(QString strGroup, QString strKey, QString strDefaultValue);
    int getConfigValue(QString strGroup, QString strKey, int nDefaultValue);
    double getConfigValue(QString strGroup, QString strKey, double fDefaultValue);
    QString getProvinceNameFromConfig();

    void setConfigValue(QString strGroup, QString strKey, QString strValue);
    void setConfigValue(QString strGroup, QString strKey, int nValue);
    void setConfigValue(QString strGroup, QString strKey, double fValue);

private:
    static QSettingsConfig *self;
    explicit QSettingsConfig(QObject *parent = nullptr);
    ~QSettingsConfig();

private:
    QString m_strProvinceName;
    int m_nStartMsec;
    int m_nEndMsec;
    QMutex m_mutex;
};

#endif // QSETTINGSCONFIG_H
