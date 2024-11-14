#ifndef AINISETTING_H
#define AINISETTING_H
//------------------------------------------------------------------
#include <QObject>
#include <QMap>
#include <QVariant>
#include <QSettings>
//------------------------------------------------------------------
class AIniSetting : public QObject
{
	Q_OBJECT
public:
	explicit AIniSetting(QString sIniFile);
	virtual ~AIniSetting();

	QVariant value(const QString & sKey,const QVariant &sDefValue= QVariant());
    QString getCfgValue(const QString & section,const QString & key,const QString def="");
    int getCfgValue(const QString & section,const QString & key,const int def=0);
    double getCfgValue(const QString & section,const QString & key,const double def=0);
	QString getCfgComment(const QString& section, const QString& key, const QString def="");
    bool setCfgValue(const QString & section,const QString & key,const QString &value,const QString sComment="");
	void delSection(const QString& section);
	void delSectionKey(const QString& section, const QString& key);
	int getSectionCfg(QString sSection,QMap<QString,QString> & stCfgData);
	QStringList getSections();
	QString getCfgFileName() const;
	void reloadCfgFile();
    QString getAllConfigInfo();

protected:
	QMap<QString,QVariant> m_qmKeyValue;		//[小写Key]=value
	QMap<QString,QString> m_qmSensitiveKey;		//[小写Key]=大小写敏感Key
	QMap<QString,QString> m_qmKeyComment;		//[敏感Key]=注释
	QStringList m_slSections;					//[节点列表]

	QString m_sCfgFile;
	void LoadCfgFromINI();
private:
	QString myToString( const QVariant & value );
	void saveCfgValue(const QString & section,const QString & key,const QString &value,const QString sComment="");
	bool getInsertPos(const QString & section,const QString & key,qint64 &iPos,qint64 &iNextPos);
	bool getDeletePos(const QString & section,qint64 &iPos,qint64 &iNextPos);
	bool updateIniFile(qint64 iPos,qint64 iNextPos,const QString sSection, const QString & sLine);
signals:

public slots:
};
//------------------------------------------------------------------
#endif // AINISETTING_H
