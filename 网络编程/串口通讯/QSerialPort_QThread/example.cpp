#include "SerialThread.h"

//1、声明该变量
SerialThread *m_SerialThread;

//2、定义该变量
m_SerialThread = new SerialThread();
m_SerialThread->start();


//绑定一些信号槽
connect(m_SerialThread, &SerialThread::sigRecvOneFrame, this, &XXX::slotRecvOneFrame);
connect(m_SerialThread, &SerialThread::finished, m_SerialThread, &SerialThread::deleteLater);