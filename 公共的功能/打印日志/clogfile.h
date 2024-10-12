#ifndef CLOGFILE_H
#define CLOGFILE_H

#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include <QMutex>

#define IFLOG_MAX_FILE_SIZE            10L*1024*1024

void RecordLogFile(QString strLog);


#endif // CLOGFILE_H
