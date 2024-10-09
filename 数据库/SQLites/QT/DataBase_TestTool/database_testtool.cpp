#include "database_testtool.h"
#include "ui_database_testtool.h"

#include<QPushButton>
#include<QLibrary>
#include<QDebug>
#include <QUuid>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QThread>

#include "robotdatabaseapi.h"

#define DB_NAME  "trawe.db"

DataBase_TestTool::DataBase_TestTool(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataBase_TestTool)
{
    ui->setupUi(this);
    ui->textEdit_sql->setLineWrapMode(QTextEdit::WidgetWidth);
    ui->btn_connectDB->setCheckable(true);

    m_pThread = new QThread();
    m_pDataCenter = new DataCenter();
    m_pDataCenter->moveToThread(m_pThread);

    qRegisterMetaType<enDbOperationType>("enDbOperationType");
    qRegisterMetaType<string>("string");

    m_pThread->start();

    connect(this, &DataBase_TestTool::sigConnect, m_pDataCenter, &DataCenter::slotConnectDB);
    connect(this, &DataBase_TestTool::sigDisconnected, m_pDataCenter, &DataCenter::slotDisconnected);
    connect(this, &DataBase_TestTool::sigAddData, m_pDataCenter, &DataCenter::slotAddData);
    connect(this, &DataBase_TestTool::sigAddMoreData, m_pDataCenter, &DataCenter::slotAddMoreData);
    connect(this, &DataBase_TestTool::sigDeleteDataById, m_pDataCenter, &DataCenter::slotDeleteDataById);
    connect(this, &DataBase_TestTool::sigUpdateDataById, m_pDataCenter, &DataCenter::slotUpdateDataById);
    connect(this, &DataBase_TestTool::sigQueryDataById, m_pDataCenter, &DataCenter::slotGetDataById);
    connect(this, &DataBase_TestTool::sigGetDbCount, m_pDataCenter, &DataCenter::slotGetDbCount);

    connect(m_pDataCenter, &DataCenter::sigStatus, this, &DataBase_TestTool::slotGetStatusFromDB);
    connect(m_pDataCenter, &DataCenter::sigCount, this, &DataBase_TestTool::slotGetDbCount);
    connect(m_pDataCenter, &DataCenter::sigQueryDataById, this, &DataBase_TestTool::slotGetQueryDataById);

}

DataBase_TestTool::~DataBase_TestTool()
{
    delete ui;
    emit sigDisconnected();
}

void DataBase_TestTool::on_btn_connectDB_clicked()
{
    if(ui->btn_connectDB->isChecked())
    {
        qDebug() << __LINE__ << __FUNCTION__ << "开始连接数据库:" ;
        emit sigConnect();
    }
    else
    {
        qDebug() << __LINE__ << __FUNCTION__ << "开始断开数据库:" ;
        emit sigDisconnected();
    }

}

void DataBase_TestTool::createOrederId()
{
    QUuid id = QUuid::createUuid();
    QString strId = id.toString();
    ui->lineEdit_orderId->setText(strId);
}

void DataBase_TestTool::on_btn_add_clicked()
{
    qDebug() << __LINE__ << __FUNCTION__ << "开始添加数据:" ;

    tagOrderParam tempParam;
    tempParam.strOrderID = ui->lineEdit_orderId->text().toStdString();
    tempParam.strEntry = ui->lineEdit_entry->text().toStdString();
    tempParam.strMoney = ui->lineEdit_money->text().toStdString();
    tempParam.strPlate = ui->lineEdit_plate->text().toStdString();
    tempParam.strType = ui->comboBox_type->currentText().toStdString();
    tempParam.strWeight = ui->lineEdit_weight->text().toStdString();

    emit sigAddData(tempParam.strOrderID, tempParam.strEntry, tempParam.strMoney,
                          tempParam.strPlate, tempParam.strType, tempParam.strWeight);
}

void DataBase_TestTool::on_btn_query_clicked()
{
    qDebug() << __LINE__ << __FUNCTION__ << "开始查询数据:" ;

    string orderId = ui->lineEdit_queryOrderID->text().toStdString();
    emit sigQueryDataById(orderId);
}

void DataBase_TestTool::on_btn_showAll_clicked()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "Connection1");
    db.setDatabaseName("trawe.db"); // 数据库文件名


    if (!db.open())
    {
        qDebug() << "Failed to connect to database 1.";
        return;
    }

    QSqlQuery query(db);
    QString selectQuery = "SELECT * FROM orderData";
    if (!query.exec(selectQuery))
    {
        qDebug() << "Failed to select data from trawe.db.";
        return;
    }

    QSqlTableModel * model = new QSqlTableModel(this, db);
    model->setTable("orderData");
    ui->tableView->setModel(model);
    model->select();

    QStringList tables;
    tables << "id"<< "订单id" << "入口" << "金额" << "车牌" << "车型" << "重量" << "时间";
    for(int i = 1 ; i < tables.count(); i++)
        model->setHeaderData(i,Qt::Horizontal,tables[i]);//设置显示框表头显示
    model->setSort(1, Qt::AscendingOrder);//设置按照第0列排序
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//设置单元格不可编辑
    ui->tableView->horizontalHeader()->setStretchLastSection(true);//设置最后一列填充后面表格
    //ui->tableView->setColumnHidden(0,true);//设置第0行隐藏
    ui->tableView->setColumnWidth(1,100);//设置列宽，界面看起来更舒适
    ui->tableView->setColumnWidth(2,100);//设置列宽，界面看起来更舒适

    db.close();
}

void DataBase_TestTool::on_btn_delete_clicked()
{
    qDebug() << __LINE__ << __FUNCTION__ << "开始删除数据:" ;
    string orderId = ui->lineEdit_orderIdDelete->text().toStdString();
    emit sigDeleteDataById(orderId);
}

void DataBase_TestTool::on_btn_update_clicked()
{
    qDebug() << __LINE__ << __FUNCTION__ << "开始更新数据:" ;


    tagOrderParam tempParam;

    tempParam.strOrderID = ui->lineEdit_orderIdUpdate->text().toStdString();
    tempParam.strEntry = ui->lineEdit_entryUpdate->text().toStdString();
    tempParam.strMoney = ui->lineEdit_moneyUpdate->text().toStdString();
    tempParam.strPlate = ui->lineEdit_plateUpdate->text().toStdString();
    tempParam.strType = ui->comboBox_typeUpdate->currentText().toStdString();
    tempParam.strWeight = ui->lineEdit_weightUpdate->text().toStdString();

    emit sigUpdateDataById(tempParam.strOrderID, tempParam.strEntry, tempParam.strMoney,
                             tempParam.strPlate, tempParam.strType, tempParam.strWeight);
}

/*
(1)INSERT INTO orderData (orderId, entry, money, plate, type, weight) VALUES ("63945", "滘口", "99", "粤A777Q1", "客一", "1800");
(2)UPDATE orderData SET entry = "滘口", money = "99", plate = "粤A777Q1", type = "客一", weight = "客一" WHERE orderId="22834";
(3)DELETE FROM orderData WHERE orderId=1234567;
*/

void DataBase_TestTool::on_btn_queryExec_clicked()
{
    qDebug() << __LINE__ << __FUNCTION__ << "暂时屏蔽该功能" ;
    ui->label_execSqlStatus->setText("该功能不可用");
    return;

    qDebug() << __LINE__ << __FUNCTION__ << "开始执行sql语句:" ;

    string sql = ui->textEdit_sql->toPlainText().toStdString();

    bool ret = DB_queryExec(sql);

    if(ret)
    {
        qDebug() << "执行sql语句成功！";
        ui->label_execSqlStatus->setText("执行成功！");
    }
    else
    {
        qDebug() << "执行sql语句失败！";
        ui->label_execSqlStatus->setText("执行失败！");
    }
}

void DataBase_TestTool::on_btn_addDataMore_clicked()
{
    ui->label_addStatusMore->setText("添加中...");

    int count = ui->lineEdit_count->text().toInt();
    int index = ui->lineEdit_index->text().toInt();

    emit sigAddMoreData(count, index);
}

void DataBase_TestTool::on_btn_deleteAll_clicked()
{
    qDebug() << __LINE__ << __FUNCTION__ << "暂时屏蔽该功能" ;
    ui->label_deleteAllStatus->setText("该功能不可用");
    return;

    qDebug() << __LINE__ << __FUNCTION__ << "开始删除整个表的数据" ;


    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "Connection1");
    db.setDatabaseName(DB_NAME); // 数据库文件名

    if (!db.open())
    {
        qDebug() << "Failed to connect to database 1.";
        return;
    }

    QSqlQuery query(db);
    QString sql = "DELETE FROM orderData";
    if (!query.exec(sql))
    {
        qDebug() << "Failed to delete data from trawe.db.";
        return;
    }

    ui->label_deleteAllStatus->setText("删除成功");
    db.close();
}

void DataBase_TestTool::on_btn_dbTotalCount_clicked()
{
    qDebug() << __LINE__ << __FUNCTION__ << "开始查询整个表的数据总数" ;
    emit sigGetDbCount();
}

void DataBase_TestTool::slotGetStatusFromDB(enDbOperationType type, bool flag)
{
    QString strLog;
    switch (type)
    {
    case enDbOperationType::connect:
    {
        if(flag)
        {
            strLog = "连接数据库成功！" ;
            ui->btn_connectDB->setText("断开数据库");
        }
        else
        {
            strLog = "连接数据库失败！";
            ui->btn_connectDB->setChecked(false);
            ui->btn_connectDB->setText("连接数据库");
        }
        ui->btn_connectDB->setChecked(flag);
        ui->label_connectStatus->setText(strLog);
    }
        break;
    case enDbOperationType::disConnected:
    {
        if(flag)
        {
            strLog = "断开数据库成功!" ;
            ui->btn_connectDB->setText("连接数据库");
        }
        else
        {
            strLog = "断开数据库失败!";
            ui->btn_connectDB->setChecked(false);
            ui->btn_connectDB->setText("断开数据库");
        }

        ui->label_connectStatus->setText(strLog);
    }
        break;
    case enDbOperationType::addData:
    {
        flag? strLog = "添加成功" :strLog = "添加失败";
        ui->label_addDataStatus->setText(strLog);
    }
        break;
    case enDbOperationType::addMoreData:
    {
        flag? strLog = "添加成功" :strLog = "添加失败";
        ui->label_addStatusMore->setText(strLog);
    }
        break;
    case enDbOperationType::deleteData:
    {
        flag? strLog = "删除成功！" :strLog = "删除失败！";
        ui->label_deleteDataStatus->setText(strLog);
    }
        break;
    case enDbOperationType::updateData:
    {
        flag? strLog = "更新成功！" :strLog = "更新失败！";
        ui->label_updateDataStatus->setText(strLog);
    }
        break;
    }

    qDebug() << strLog;
}


void DataBase_TestTool::slotGetDbCount(int count)
{
    qDebug() << "整个表的数据总数:" << QString::number(count);
    ui->label_dbTotalCount->setText(QString::number(count));
}

void DataBase_TestTool::slotGetQueryDataById(bool ret, string strEntry, string strMoney, string strPlate, string strType, string strWeight)
{
    if(ret)
    {
        qDebug() << "查询数据成功！";
        ui->label_queryStatus->setText("查询成功！");
        qDebug() << "入口信息" << QString::fromStdString(strEntry);

        ui->label_entryQuery->setText(QString::fromStdString(strEntry));
        ui->label_moneyQuery->setText(QString::fromStdString(strMoney));
        ui->label_plateQuery->setText(QString::fromStdString(strPlate));
        ui->label_typeQuery->setText(QString::fromStdString(strType));
        ui->label_weightQuery->setText(QString::fromStdString(strWeight));
    }
    else
    {
        qDebug() << "查询数据失败！";
        ui->label_queryStatus->setText("查询失败！");
    }
}
