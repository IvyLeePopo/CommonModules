#ifndef ZHTOPY_H
#define ZHTOPY_H

#include <QObject>
#include <QStringList>

class ZhToPY : public QObject
{
    Q_OBJECT
public:
    static ZhToPY *Instance();
    explicit ZhToPY(QObject *parent = 0);

private:
    static ZhToPY *self;
    QStringList listPY;
    QStringList listJP;

public:
    QString zhToPY(const QString &chinese);
    QString zhToJP(const QString &chinese);
};

#endif // ZHTOPY_H
