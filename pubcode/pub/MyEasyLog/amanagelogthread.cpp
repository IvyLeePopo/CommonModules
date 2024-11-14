#include "amanagelogthread.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include "QtGui/private/qzipreader_p.h"
#include "QtGui/private/qzipwriter_p.h"

//------------------------------------------------------------------
AManageLogThread::AManageLogThread():QThread(NULL)
{
    m_iMode = LOGMANAGEMODE::LMM_AUTO;
    m_sDelJudgeHour = "02";
	m_bStop = false;
	m_dtLastCheck = QDateTime::currentDateTime().addDays(-1);
}
//------------------------------------------------------------------
AManageLogThread::~AManageLogThread()
{
	m_bStop = true;
	wait();
}
//------------------------------------------------------------------
AManageLogThread &AManageLogThread::getInstance()
{
    static AManageLogThread ins;
	if(ins.isRunning()==false)
		ins.start();

	return ins;
}

//------------------------------------------------------------------
void AManageLogThread::setManageLogInfo(QString sLogPath, int iStayDay, bool bNeedZip, QString Filters)
{
    m_qmLogStayDay[sLogPath] = iStayDay;
    m_qmLosNeedZipFlag[sLogPath] = bNeedZip;
    m_qmLosFilters[sLogPath] = Filters;
}

void AManageLogThread::setManageLogMode(int nMode, QString sDelJudgeHour)
{
    m_iMode = nMode;
    m_sDelJudgeHour = sDelJudgeHour;
}

void AManageLogThread::enableExecManageLog()
{
    m_bAllowExecManageLog = true;
}

//------------------------------------------------------------------
void AManageLogThread::run()
{
	while(m_bStop==false)
	{
        //自动执行压缩和超时删除日志
        if(LMM_AUTO == m_iMode)
        {
            QDateTime dtNow = QDateTime::currentDateTime();
            if(dtNow.toString("yyyy-MM-dd")!=m_dtLastCheck.toString("yyyy-MM-dd"))
            {
                //默认每天凌晨2点执行压缩和超时删除日志,可配置
                if(dtNow.toString("hh") == m_sDelJudgeHour)
                {
                    dealWithLog();
                    m_dtLastCheck = dtNow;
                }
            }
        }

		//休眠30秒
		for(int i=0;i<300;i++)
		{
            //手动触发执行压缩和超时删除日志文件
            if(LMM_MANUAL == m_iMode && m_bAllowExecManageLog)
            {
                dealWithLog();
                m_bAllowExecManageLog = false;
            }

			if(m_bStop==false)
				msleep(100);
		}
    }
}

void AManageLogThread::dealWithLog()
{
    QMap<QString,int>::iterator it;
    for(it=m_qmLogStayDay.begin();it!=m_qmLogStayDay.end();++it)
    {
        QString sSavePath = it.key();
        int nSaveDays = it.value();
        bool bNeedZip = m_qmLosNeedZipFlag.value(sSavePath);
        QString sFilters = m_qmLosFilters.value(sSavePath);
        qInfo() << "Log SavePath:" << sSavePath;
        qInfo() << "Log SaveDays:" << nSaveDays;
        qInfo() << "Log NeedZip:" << bNeedZip;
        qInfo() << "Log Filters:" << sFilters;

        //注意这里先删除日志，再压缩，避免在用的很多日志先压缩后无法执行实际日志的超时删除
        //删除超时日志
        QStringList filtersDelLog = sFilters.split("|");
//        filtersDelLog << "*.log" << "*.zip";
        delOverTimeLog(sSavePath,filtersDelLog,nSaveDays);

        if(bNeedZip)
        {
            //压缩日志判断时间
            QDateTime dtThen = QDateTime::currentDateTime().addDays(-1); //保存nSaveDays天

            QStringList filters = sFilters.replace("*.zip","").replace("*.rar","").replace("*.tar.gz","").replace("||","|").split("|");
//            filters << "*.log";

            QDir dir(sSavePath);
            dir.setFilter(QDir::Files);
            dir.setNameFilters(filters);
            dir.setSorting(QDir::Time);
            QFileInfoList list = dir.entryInfoList();
            for (int i = list.size() - 1; i >=0; i--)
            {
                QFileInfo fileInfo = list.at(i);
                if(fileInfo.lastModified().toString("yyyy-MM-dd")<=dtThen.toString("yyyy-MM-dd"))
                {
                    QString sCompressFile = fileInfo.absoluteFilePath();
                    QString sCompressZipFile = sCompressFile;
                    qInfo() << QString("压缩文件:%1").arg(sCompressFile);
                    qInfo() << QString("压缩后文件:%1").arg(sCompressZipFile.replace(".log",".zip"));
                    compress(sCompressFile,sCompressZipFile,true);
                }
                //释放下CPU
                QThread::msleep(10);
            }
        }
    }
}

bool AManageLogThread::compress(const QString &sFile, const QString &sZipFile, bool bDeleteFile)
{
    QFile file(sFile);
    if(!file.exists())
    {
        qWarning()<< "file not exists:" << sFile;
        return false;
    }

    QString sZipPath = sZipFile;
    int nIndex = -1;
    nIndex =sZipPath.lastIndexOf("\\");
    if(-1 == nIndex)
        nIndex =sZipPath.lastIndexOf("/");
    if(-1 != nIndex)
        sZipPath = sZipPath.left(nIndex);

//    qDebug()<< "ZipPath:" << sZipPath;

    QDir dir(sZipPath);
    if(false == dir.exists())
    {
        QDir dircur;
        dircur.mkpath(sZipPath);
    }

    QZipWriter writer(sZipFile);
//    QZipWriter::CompressionPolicy policy = writer.compressionPolicy();
//    qDebug()<< "policy:" << policy;
    writer.setCompressionPolicy(QZipWriter::AutoCompress);

    QFileInfo fileInfo(sFile);
    file.open(QIODevice::ReadOnly);
    writer.addFile(fileInfo.fileName(), file.readAll());
    file.close();
    writer.close();
    if(bDeleteFile)
        file.remove();

    return true;
}

void AManageLogThread::delOverTimeLog(const QString &sSavePath, QStringList filters, int nSaveDays)
{
    //删除超时日志
    QDateTime dtLastCheckTime = QDateTime::currentDateTime().addDays(-1);

    if(dtLastCheckTime.daysTo(QDateTime::currentDateTime()) > 0)
    {
        QDateTime dtThen = QDateTime::currentDateTime().addDays(0-nSaveDays); //保存nSaveDays天

        QDir dir(sSavePath);
        dir.setFilter(QDir::Files);
        dir.setNameFilters(filters);
        dir.setSorting(QDir::Time);
        QFileInfoList list = dir.entryInfoList();
        for (int i = list.size() - 1; i >=0; i--)
        {
            QFileInfo fileInfo = list.at(i);
            if(fileInfo.lastModified().toString("yyyy-MM-dd")<=dtThen.toString("yyyy-MM-dd"))
            {
                QString sDelFile = fileInfo.absoluteFilePath();
                QFile::remove(sDelFile);

                qInfo() << QString("delete over time file:%1").arg(sDelFile);
            }
            else
                break;
        }

        dtLastCheckTime = QDateTime::currentDateTime();
    }
}
