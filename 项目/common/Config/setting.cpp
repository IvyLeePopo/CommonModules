#include "setting.h"

QMutex *Setting::mLock = nullptr;       //读写锁指针
QSettings *Setting::iniSet = nullptr;   //ini文件指针

Setting::Setting()
{

}

Setting::~Setting()
{
    delete iniSet;
    delete mLock;
}

// 创建.ini文件
void Setting::init()
{
    iniSet = new QSettings("./Config.ini", QSettings::IniFormat); 
    mLock = new QMutex;
}

// 获取配置参数
QVariant Setting::value(const QString &key) 
{
    mLock->lock();
    QVariant value = iniSet->value(key);
    mLock->unlock();

    return  value;
}

// 写入参数
void Setting::setValue(const QString &key, const QVariant &value)    
{
    mLock->lock();
    iniSet->setValue(key,value);
    iniSet->sync();
    mLock->unlock();
}

void Setting::sync()
{
    iniSet->sync();
}
