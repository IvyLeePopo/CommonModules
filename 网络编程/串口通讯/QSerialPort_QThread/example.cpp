#include "SerialThread.h"

//1�������ñ���
SerialThread *m_SerialThread;

//2������ñ���
m_SerialThread = new SerialThread();
m_SerialThread->start();


//3����һЩ�źŲ�
connect(m_SerialThread, &SerialThread::sigRecvOneFrame, this, &XXX::slotRecvOneFrame);
connect(m_SerialThread, &SerialThread::finished, m_SerialThread, &SerialThread::deleteLater);

//4�����ڷ������ݣ�
QByteArray strBeepCmd = QByteArray::fromRawData("\nB\r", 3);
m_SerialThread->WriteCom(strBeepCmd);

//5�����ڽ�������
emit sigRecvOneFrame(QByteArray byteArray);