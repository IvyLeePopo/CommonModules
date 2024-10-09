#include "database_testtool.h"
#include <QApplication>

#include <QMutex>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QDir>

static QMutex g_qmutex; // 互斥锁
QString g_LogDirPath = "/var/log/";
int level = 4;

void outputMessage(QtMsgType type, const QMessageLogContext& Context, const QString &data )
{
    if(g_qmutex.tryLock(10)==false)
    {
        return;
    }

    QString msg(data);
    QString text;
    int outputLevel = 0;

    switch(static_cast<int>(type))
    {
    case QtDebugMsg:
    {
        text = QString("Debug:");
        outputLevel = 4;
    }
        break;
    case QtWarningMsg:
    {
        text = QString("Warning:");
        outputLevel = 3;
    }
        break;
    case QtCriticalMsg:
    {
        text = QString("Critical:");
        outputLevel = 2;
    }
        break;
    case QtFatalMsg:
    {
        text = QString("Fatal:");
        outputLevel = 1;
    }
        break;
    }

    if(level<outputLevel)  //dang qian ji bie  xiao yu  shu chu ji bie
    {
        return;
    }

    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString current_date = QString("[%1]").arg(current_date_time);
    QString message = QString("%1 %2 %3").arg(current_date).arg(text).arg(msg);

    QString file_time = QDate::currentDate().toString("yyyyMMdd");
    QString dir = QDir::currentPath();
    QString file_name1  = QString(dir/*g_LogDirPath*/+"LogDataBase%1.log").arg(file_time);

    QFile file(file_name1);
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    file.flush();
    file.close();
    g_qmutex.unlock();

    (void)Context;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));
    DataBase_TestTool w;
    w.show();

    qInstallMessageHandler(outputMessage);

    return a.exec();
}
