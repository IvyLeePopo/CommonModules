#ifndef INISETTING_H
#define INISETTING_H

#include <QObject>
#include <QSettings>
#include <QMutex>

class IniSetting
{
public:
    IniSetting();
    ~IniSetting();
    static void init();
    static void setValue(const QString &key,const QVariant &value);
    static QVariant value(const QString &key);
    static void sync();

private:
    static QSettings *iniSet;
    static QMutex *mLock;
};

#endif // INISETTING_H
