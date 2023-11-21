#include "SerialThread.h"

//1、声明该变量
SerialThread *m_SerialThread;

//2、定义该变量
m_SerialThread = new SerialThread();
m_SerialThread->start();


//3、绑定一些信号槽
connect(m_SerialThread, &SerialThread::sigRecvOneFrame, this, &XXX::slotRecvOneFrame);
connect(m_SerialThread, &SerialThread::finished, m_SerialThread, &SerialThread::deleteLater);

//4、串口发送数据：
QByteArray strBeepCmd = QByteArray::fromRawData("\nB\r", 3);
m_SerialThread->WriteCom(strBeepCmd);

//5、串口接收数据
emit sigRecvOneFrame(QByteArray byteArray);