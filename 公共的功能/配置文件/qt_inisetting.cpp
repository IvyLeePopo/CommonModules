#include "qt_inisetting.h"

QMutex *IniSetting::mLock = nullptr;       //读写锁指针
QSettings *IniSetting::iniSet = nullptr;   //ini文件指针

IniSetting::IniSetting()
{

}

IniSetting::~IniSetting()
{
    delete iniSet;
    delete mLock;
}

void IniSetting::init()
{
    iniSet = new QSettings("./RobotConfig.ini", QSettings::IniFormat); //创建.ini文件
    mLock = new QMutex;
}

QVariant IniSetting::value(const QString &key)  //获取配置参数
{
    mLock->lock();
    QVariant value = iniSet->value(key);
    mLock->unlock();

    return  value;
}

void IniSetting::setValue(const QString &key, const QVariant &value)    //写入参数
{
    mLock->lock();
    iniSet->setValue(key,value);
    iniSet->sync();
    mLock->unlock();
}

void IniSetting::sync()
{
    iniSet->sync();
}
