#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

#include "WebSocketServer.h"
#include "ReaderWebsocket/readerwebsocketserver.h"

// websocket客户端发来的数据，转发给串口
void RecvMsgToSerial(const char* pszData, unsigned int nSize)
{
    if (NULL == pszData)
        return;

    //todo
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    startServer();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startServer()
{
    ReaderWebsocketServer::getInstance();
    ReaderWebsocketServer::getInstance()->setCallBack(RecvMsgToSerial);
}
