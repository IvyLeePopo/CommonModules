#include "SerialThread.h"

//1�������ñ���
SerialThread *m_SerialThread;

//2������ñ���
m_SerialThread = new SerialThread();
m_SerialThread->start();


//��һЩ�źŲ�
connect(m_SerialThread, &SerialThread::sigRecvOneFrame, this, &XXX::slotRecvOneFrame);
connect(m_SerialThread, &SerialThread::finished, m_SerialThread, &SerialThread::deleteLater);