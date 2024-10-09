#include "robotdatabasemanager.h"
#define DB_NAME  "qt_sql_default_connection"

RobotDataBaseManager::RobotDataBaseManager()
{
    dbDriver = "QSQLITE";
    //dbName = "/opt/scaner/lib/trawe.db";//linux的目录
    dbName = "trawe.db";//windows的测试目录
    userName = "trawe";
    userPwd = "trawe";
    hostName = "localhost";
    hostPort = 5432;
    m_bConnected = false;
    qDebug() << "Db instance inited...";

    //LinuxTimerManager::initLinuxTimer(m_deleteTimerId,deleteTimerProc, "RobotDataBaseManager-m_deleteTimerId");
    //LinuxTimerManager::startLinuxTimer(m_deleteTimerId,30,0);
}

RobotDataBaseManager::~RobotDataBaseManager()
{
    closeDB();
    m_bConnected = false;
    db.removeDatabase(DB_NAME);

}

void RobotDataBaseManager::deleteTimerProc(timer_t data)
{
    QSqlQuery query;
    QString sql = "delete  from orderData where datetime(logtime)<= datetime('now','-20 minute','localtime')";
    query.prepare(sql);
    if (!query.exec())
    {
        qDebug() << " [#RobotDataBaseManager deleteTimerProc#] ERROR: " << query.lastError().text();
        return ;
    }
}

bool RobotDataBaseManager::checkColumnExists2()
{
    QString strSql = QString::fromLocal8Bit("select * from sqlite_master where name='orderData' and sql like '%logtime%'");
    QSqlQuery query;
    query.exec(strSql);
    if (!query.next())// 无SuccessRate字段
    {
        // TODO:
        qDebug()<<"have not this session";
        return false;
    }
    else
    {
        qDebug()<<"have this session";
        return true;
    }

}

bool RobotDataBaseManager::connectDB()
{
    m_bConnected = openDB();
    return m_bConnected;
}

bool RobotDataBaseManager::disConnectDB()
{
    closeDB();
    m_bConnected = false;

    return true;
}

bool RobotDataBaseManager::connDB()
{

    qDebug() << "Db::connDb -> " << hostName << "/" << dbName;

    if(QSqlDatabase::contains(DB_NAME))
        db = QSqlDatabase::database(DB_NAME);
    else
        db = QSqlDatabase::addDatabase(dbDriver);
    db.setHostName(hostName);
    db.setDatabaseName(dbName);
    db.setUserName(userName);
    db.setPassword(userPwd);

    QString strlog;
    QFile file2(dbName);
    if(file2.exists())
    {
        QString str=  "chmod 777 " + dbName;
        system(str.toLocal8Bit().data());
    }

    // create connection
    if (!db.open())
    {
        strlog = "[数据库订单模块]，数据库打开失败，错误代码："+db.lastError().text();
        qDebug() << strlog;

        return false;
    }
    QFile file(dbName);
    if(file.exists())
    {
        bool isExistLogtime = checkColumnExists2();
        if(isExistLogtime==false)
        {
            QSqlQuery query;
            query.exec(">DROP TABLE orderData ");
            qDebug() << "RobotDataBaseManager::connDB  DROP TABLE orderData";

        }
        else
        {
            QString str=  "chmod 777 " + dbName;
            system(str.toLocal8Bit().data());
        }
    }

    bool breturn;

    //建立数据库表
    QSqlQuery query;
    breturn = query.exec("create table orderData(id integer not null primary key autoincrement,       \
                         orderId text,                                                                        \
                         entry text,                                                                         \
                         money text,                                                                          \
                         plate text,                                                                          \
                         type text,                                                                          \
                         weight text,                                                                \
                         logtime TIMESTAMP default (datetime('now', 'localtime'))  ) ");

    if (breturn)
    {
        strlog.clear();
        strlog = "[数据库订单模块]，建立数据库表成功";
    }
    else
    {
        strlog.clear();
        strlog = "[数据库订单模块]，数据库表已存在";
    }
    qDebug() << "RobotDataBaseManager::connDB" + strlog;

    return true;
}

void RobotDataBaseManager::closeDB()
{
    if (db.isOpen())
    {
        qDebug() << "------close db------: ";
        db.close();
    }
}

bool RobotDataBaseManager::openDB()
{
    return connDB();
}

QSqlDatabase RobotDataBaseManager::database()
{
    return db;
}

bool RobotDataBaseManager::addData2DB(string strOrderId, string strEntry, string strMoney, string strPlate, string strType, string strWeight)
{
    if(!bConnected())
        return false;

    if (isOrderIdExist(strOrderId))
        return false;

    QString orderId = QString::fromStdString(strOrderId);
    QString entry = QString::fromStdString(strEntry);
    QString money = QString::fromStdString(strMoney);
    QString plate = QString::fromStdString(strPlate);
    QString type = QString::fromStdString(strType);
    QString weight = QString::fromStdString(strWeight);

    QSqlQuery query;
    QString sql = "INSERT INTO orderData (orderId, entry, money, plate, type, weight) VALUES (:orderId, :entry, :money, :plate, :type, :weight);";

    query.prepare(sql);
    query.bindValue(":orderId", orderId);
    query.bindValue(":entry", entry);
    query.bindValue(":money", money);
    query.bindValue(":plate", plate);
    query.bindValue(":type", type);
    query.bindValue(":weight", weight);

    if (!query.exec())
    {
        qDebug()<<tr("订单Id %1 ，加入缓存失败：%2").arg(orderId).arg(query.lastError().text());
        return false;
    }

    //qDebug()<<tr("订单Id %1 已加入缓存").arg(orderId);

    return true;
}

QString RobotDataBaseManager::getCurrentOfCancelPayId()
{
    QString payId;

    if(m_mutex4OrderId.tryLock(1*1000)==false)
        return "";

    payId = m_curOrderId;
    m_curOrderId.clear();
    m_mutex4OrderId.unlock();

    return payId;
}

int RobotDataBaseManager::getTypeById(string id)
{
    qDebug() << "[数据库]:getTypeById: 开始~~~~~~~~~~~~~~~~~";

    int type = 1;
    QString orderId = QString::fromStdString(id);
    QSqlQuery query;
    QString sql = "SELECT type FROM orderData WHERE orderId=:orderId;";

    query.exec("SELECT * FROM my_table");

    query.prepare(sql);
    query.bindValue(":orderId", orderId);

    if (!query.exec())
    {
        return type;
    }

    while (query.next())
    {
        int type = query.value(0).toInt();
        qDebug() << "取得1条订单缓存数据: " << type << "/" << orderId;
        return type;
    }

    qDebug() << "[数据库]:getTypeById~~~~~~: " << type << "/" << orderId;

    return type;
}


bool RobotDataBaseManager::getDataById(string strOrderId, string& strEntry, string& strMoney, string& strPlate, string& strType, string& strWeight)
{
    if(!bConnected())
        return false;

    qDebug() << "[数据库]:getDataById 查询所有数据: 开始~~~~~~~~~~~~~~~~~";

    QString orderId = QString::fromStdString(strOrderId);

    QSqlQuery query;
    QString sql = "SELECT * FROM orderData WHERE orderId=:orderId;";
    query.prepare(sql);
    query.bindValue(":orderId", orderId);

    if (!query.exec())
    {
        qDebug() << "into getDataById ERROR: " << query.lastError().text();
        return false;
    }

    while (query.next())
    {
        QString orderId = query.value("orderId").toString();

        qDebug() << "取得1条订单缓存数据: " << orderId;
        if (!orderId.isEmpty())
        {
            strEntry = query.value("entry").toString().toStdString();
            strMoney = query.value("money").toString().toStdString();
            strPlate = query.value("plate").toString().toStdString();
            strType = query.value("type").toString().toStdString();
            strWeight = query.value("weight").toString().toStdString();
        }
    }

    qDebug() << "[数据库]:getDataById 查询所有数据: 结束~~~~~~~~~~~~~~~~~";
    return true;
}

bool RobotDataBaseManager::queryExec(string strSql)
{
    if(!bConnected())
        return false;

    QString sql = QString::fromStdString(strSql);

    QSqlQuery query;
    query.prepare(sql);
    if (!query.exec())
    {
        qDebug() << " queryExec ERROR: " << query.lastError().text();
        return false;
    }

    qDebug() << "[数据库]: sql 语句执行成功: 结束~~~~~~~~~~~~~~~~~";
    return true;
}

int RobotDataBaseManager::countDB()
{
    if(!bConnected())
        return false;

    QSqlQuery query;
    QString sql = "SELECT COUNT(*) FROM orderData;";
    query.prepare(sql);

    int count = 0;
    if (!query.exec())
    {
        qDebug() << "into countDB ERROR: " << query.lastError().text();
        return false;
    }

    while (query.next())
    {
        count = query.value(0).toInt();
        // 在这里使用count变量
        qDebug() << "从数据表查出的总数Count: " << count;
    }

    return count;
}

bool RobotDataBaseManager::DeleteDataById(string strOrderId)
{
    if(!bConnected())
        return false;

    qDebug() << "RobotDataBaseManager::DeleteDataById: " << QString::fromStdString(strOrderId);

    QString orderId = QString::fromStdString(strOrderId);

    QSqlQuery query;
    QString sql = "DELETE FROM orderData WHERE orderId=:orderId;";
    query.prepare(sql);
    query.bindValue(":orderId", orderId);
    if (!query.exec())
    {
        qDebug() << "into DeleteDataById ERROR: " << query.lastError().text();
        return false;
    }

    if(m_mutex4OrderId.tryLock(1*1000)==false)
        return false;

    m_curOrderId = orderId;
    m_mutex4OrderId.unlock();

    return true;
}

bool RobotDataBaseManager::UpdateDataById(string strOrderId, string strEntry, string strMoney, string strPlate, string strType, string strWeight)
{
    if(!bConnected())
        return false;

    if (!isOrderIdExist(strOrderId))
        return false;

    QString orderId = QString::fromStdString(strOrderId);
    QString entry = QString::fromStdString(strEntry);
    QString money = QString::fromStdString(strMoney);
    QString plate = QString::fromStdString(strPlate);
    QString type = QString::fromStdString(strType);
    QString weight = QString::fromStdString(strWeight);

    qDebug()<< "orderId: " << orderId;
    qDebug()<< "entry: " << entry;
    qDebug()<< "money: " << money;
    qDebug()<< "plate: " << plate;
    qDebug()<< "type: " << type;
    qDebug()<< "weight: " << weight;

    QSqlQuery query;
    QString sql = "UPDATE orderData SET entry = :entry, money = :money, plate = :plate, type = :type, weight = :weight WHERE orderId=:orderId;";
    query.prepare(sql);

    query.bindValue(":orderId", orderId);
    query.bindValue(":entry", entry);
    query.bindValue(":money", money);
    query.bindValue(":plate", plate);
    query.bindValue(":type", type);
    query.bindValue(":weight", weight);

    if (!query.exec())
    {
        qDebug() << "UpdateDataById ERROR: " << query.lastError().text();
        return false;
    }

    if(m_mutex4OrderId.tryLock(1*1000)==false)
        return false;

    m_curOrderId = orderId;
    m_mutex4OrderId.unlock();

    return true;
}

bool RobotDataBaseManager::isOrderIdExist(string id)
{
    QString orderId = QString::fromStdString(id);

    QSqlQuery query;
    QString sql = "SELECT orderId FROM orderData WHERE orderId=:orderId;";
    query.prepare(sql);
    query.bindValue(":orderId", orderId);
    if (!query.exec())
    {
        qDebug() << "into isOrderIdExist ERROR: " << query.lastError().text();
        return false;
    }

    while (query.next())
    {
        qDebug()<<tr("into isOrderIdExist: %1 此订单Id已存在!").arg(orderId);
        return true;
    }

    return false;
}

bool RobotDataBaseManager::bConnected()
{
    if(!m_bConnected)
        qDebug() << "数据库还没成功连接...";

    return m_bConnected;
}

