#ifndef DATABASE_TESTTOOL_H
#define DATABASE_TESTTOOL_H

#include <QWidget>
#include <string>

#include "datacenter.h"

class QThread;

using namespace std;

namespace Ui {
class DataBase_TestTool;
}


class DataBase_TestTool : public QWidget
{
    Q_OBJECT

public:

public:
    explicit DataBase_TestTool(QWidget *parent = nullptr);
    ~DataBase_TestTool();

signals:
    void sigConnect();
    void sigDisconnected();
    void sigAddData(string strOrderId, string strEntry, string strMoney, string strPlate, string strType, string strWeight);
    void sigAddMoreData(int count, int index);
    void sigDeleteDataById(string strOrderd);
    void sigUpdateDataById(string strOrderId, string strEntry, string strMoney, string strPlate, string strType, string strWeight);
    void sigQueryDataById(string strOrderId);
    void sigGetDbCount();

private slots:
    void on_btn_connectDB_clicked();
    void on_btn_add_clicked();
    void on_btn_query_clicked();
    void on_btn_showAll_clicked();
    void on_btn_delete_clicked();
    void on_btn_update_clicked();
    void on_btn_queryExec_clicked();
    void on_btn_addDataMore_clicked();
    void on_btn_deleteAll_clicked();
    void on_btn_dbTotalCount_clicked();

private slots:
    void slotGetStatusFromDB(enDbOperationType type, bool flag);
    void slotGetDbCount(int count);
    void slotGetQueryDataById(bool ret, string strEntry, string strMoney, string strPlate, string strType, string strWeight);

private:
    void createOrederId();

private:
    Ui::DataBase_TestTool *ui;

    QThread *m_pThread;
    DataCenter *m_pDataCenter;
};

#endif // DATABASE_TESTTOOL_H
