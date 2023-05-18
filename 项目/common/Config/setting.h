/*
 * @Description: ≈‰÷√Œƒº˛
 * @Author: Ivy
 * @Date: 2022-05-18 09:34:43
 * @LastEditTime: 2022-05-18 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once

#include <QObject>
#include <QSettings>
#include <QMutex>

class Setting
{
public:
    Setting();
    ~Setting();
    static void init();
    static void setValue(const QString &key,const QVariant &value);
    static QVariant value(const QString &key);
    static void sync();

private:
    static QSettings *iniSet;
    static QMutex *mLock;
};
