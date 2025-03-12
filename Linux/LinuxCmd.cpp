#include "LinuxCmd.h"

#include <QString>
#include <QDebug>

void ShowLog(const QString strLog)
{
    qDebug() << strLog;
}

void exeCmd(const QString sCmd)
{
    QString sEcho = QString("sudo sh -c \'%1\'").arg(sCmd);
    ShowLog(QString("exec cmd: %1").arg(sEcho), false);
    system(sEcho.toUtf8().data());
}

void setGpioCtrl(QString sGpio, int ctrl_status)
{
    int fd;
    char path[64] = { 0 };

    QString strEcho = QString("/sys/class/gpio/gpio%1/value").arg(sGpio);
    ShowLog("[setLightCtrl]strEcho: " + strEcho);

    snprintf(path, sizeof(path), "%s", strEcho.toStdString().c_str());
    fd = open(path, O_WRONLY | O_NONBLOCK); // 以写方式打开
    if (fd < 0) {
        ShowLog("setLightCtrl : open gpio error !!!");
        return;
    }

    ssize_t writeLen = 0;
    //Return the number written, or -1.
    if (0 == ctrl_status) {
        writeLen = write(fd, "0", 2);
    }
    else {
        writeLen = write(fd, "1", 2);
    }

    if (writeLen < 0) //如果写入出错
    {
        ShowLog("setLightCtrl : wite gpio error !!!");
        close(fd);
        return;
    }
    else if (0 == writeLen) //如果写入0个字节，表示文件结束。
    {
        close(fd);
        ShowLog("setLightCtrl : wite zero byte data error !!!");
        return;
    }
    else //否则写入高电平或者低电平
    {
        ShowLog(QString("setLightCtrl writeLen = %1").arg(QString::number(writeLen)));
        close(fd);
        return;
    }
}

int getGpioCtrl(QString sGpio)
{
    int ctrl_status = -1;

    int fd;
    char value_str[3] = { 0 };

    QString strPath = QString("/sys/class/gpio/gpio%1/value").arg(sGpio);
    fd = open(strPath.toStdString().c_str(), O_RDONLY | O_NONBLOCK); // 以读写方式打开
    if (fd < 0) {
        qDebug() << "open gpio error !!!";
        return ctrl_status;
    }


    ssize_t readLen = read(fd, value_str, 3); //成功时，返回读取的字节数（零表示文件结束），并且文件位置按此数前进。
    if (readLen < 0) //如果读取出错
        qDebug() << "read gpio error !!!";

    else if (0 == readLen) //如果读取到0个字节，表示文件结束。
    {
        qDebug() << "read zero byte data error !!!";
    }
    else //否则读取到高电平或者低电平
    {
        if (value_str[0] == '0')   //收到号地感;
            ctrl_status = 0;
        else 
            ctrl_status = 1;
    }

    close(fd);
    return ctrl_status;
}
