#ifndef AMANAGELOGTHREAD_H
#define AMANAGELOGTHREAD_H

#include <QObject>
#include <QThread>
#include <QMap>
#include <QDateTime>

class AManageLogThread :public QThread
{
	Q_OBJECT
public:
    enum LOGMANAGEMODE
    {
        LMM_AUTO=0, LMM_MANUAL
    };

    AManageLogThread();
    ~AManageLogThread();

    static AManageLogThread &getInstance();
    void setManageLogInfo(QString sLogPath,int iStayDay = 60,bool bNeedZip = false, QString Filters = "*.log|*.txt");
    void setManageLogMode(int nMode,QString sDelJudgeHour = "02");
    void enableExecManageLog();

protected:
	virtual void run();
    //处理日志
    void dealWithLog();
    //处理压缩日志
    bool compress(const QString &sFile,const QString &sZipFile,bool bDeleteFile = false);
    //删除超时日志
    void delOverTimeLog(const QString &sSavePath,QStringList filters,int nSaveDays = 60);

private:
    QMap<QString,QString> m_qmLosFilters;   //日志目录日志需要管理的日志后缀
    QMap<QString,int> m_qmLogStayDay;       //日志目录日志对应保存天数
    QMap<QString,bool> m_qmLosNeedZipFlag;  //日志目录日志是否压缩保存
    QString m_sDelJudgeHour;                //日志管理模式自动，哪个小时执行日志的删除和压缩
    std::atomic<bool> m_bAllowExecManageLog;//允许去执行日志的删除和压缩，日志管理模式需配置为手动
    int m_iMode;                            //日志管理模式 0：自动（凌晨xx点自动执行日志的删除和压缩） 1：手动（需外部允许才去执行日志的删除和压缩）

    bool m_bStop;
	QDateTime m_dtLastCheck;
};

#endif // AMANAGELOGTHREAD_H
