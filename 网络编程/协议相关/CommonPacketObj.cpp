#include "CommonPacketObj.h"
#include <QByteArray>
#include <QTextCodec>
#include <QDebug>

CommonPacketObj::CommonPacketObj()
{

}

//UTF8转GBK(打包用)
vector<uint8> CommonPacketObj::VectorUTF8ToGBK(char *msg, int len)
{
    vector<uint8> verData;
    QByteArray inStr(msg, len);

    QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
    QTextCodec *gbk = QTextCodec::codecForName("gbk");

    QString str = utf8->toUnicode(inStr.data());
    QByteArray gbk_bytes=gbk->fromUnicode(str);

    verData.insert(verData.end(), gbk_bytes.begin(), gbk_bytes.end());
    return verData;
}

//UTF8转GBK(打包用)
QByteArray CommonPacketObj::BytesUTF8ToGBK(char *msg, int len)
{
    vector<uint8> verData;
    QByteArray inStr(msg, len);

    QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
    QTextCodec *gbk = QTextCodec::codecForName("gbk");

    QString str = utf8->toUnicode(inStr.data());
    QByteArray gbk_bytes=gbk->fromUnicode(str);

    return gbk_bytes;
}

//GBK转UTF8(解包用)
vector<uint8> CommonPacketObj::VectorGBKToUTF8(char *msg, int len)
{
    vector<uint8> verData;
    QByteArray inStr(msg, len);

    QTextCodec *gbk = QTextCodec::codecForName("gbk");
    QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");

    QString str = gbk->toUnicode(inStr.data());
    QByteArray utf8_bytes=utf8->fromUnicode(str);

    verData.insert(verData.end(), utf8_bytes.begin(), utf8_bytes.end());
    return verData;
}

//GBK转UTF8(解包用)
QByteArray CommonPacketObj::BytesGBKToUTF8(char *msg, int len)
{
    vector<uint8> verData;
    QByteArray inStr(msg, len);

    QTextCodec *gbk = QTextCodec::codecForName("gbk");
    QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");

    QString str = gbk->toUnicode(inStr.data());
    QByteArray utf8_bytes=utf8->fromUnicode(str);

    return utf8_bytes;
}

//CRC生成
vector<uint8> CommonPacketObj::CRC_16(vector<uint8> &byte, uint32 len)
{
    //qDebug() << __LINE__ << __FUNCTION__;
    uint32 i;
    uint8 Flag;
    vector<uint8> array;
    uint8 CRC16Hi = 0xFF;
    uint8 CRC16Lo = 0xFF;
    uint8 CH = 0xA0;
    uint8 CL = 0x01;
    uint8 SaveHi = 0;
    uint8 SaveLo = 0;

    for(i = 1;i < len;i++)
    {
        CRC16Lo = CRC16Lo ^ byte[i];
        for(Flag = 0; Flag < 8; Flag++)
        {
            SaveHi = CRC16Hi;
            SaveLo = CRC16Lo;
            CRC16Hi = CRC16Hi >> 1;
            CRC16Lo = CRC16Lo >> 1;
            if((SaveHi & 1) == 1)
                CRC16Lo |= 0x80;
            if((SaveLo & 1) == 1)
            {
                CRC16Hi = CRC16Hi ^ CH;
                CRC16Lo = CRC16Lo ^ CL;
            }
        }
    }
    array.push_back(CRC16Hi);
    array.push_back(CRC16Lo);
    return array;
}

//CRC校验判断
bool CommonPacketObj::CheckPackage(const vector<uint8>& vecData)
{
    uint64 package_len = vecData.size();
    vector<uint8> vecData_t(vecData);

    if (package_len < 10)
    {
        qDebug() << __LINE__ << __FUNCTION__ << "协议校验失败:长度不合法";
        return false;
    }

    if (vecData[0] != 0xDF || vecData[package_len - 1] != 0xFD)
    {
        qDebug() << __LINE__ << __FUNCTION__ << "协议校验失败:协议头尾错误";
        return false;
    }

    int len_t = (vecData[1] << 8) + vecData[2];
    uint64 package_len_t = package_len - 10;
    if (static_cast<uint64>(len_t) != package_len_t)
    {
        qDebug() << __LINE__ << __FUNCTION__ << "协议校验失败:数据域缺失" << len_t << package_len_t;
        return false;
    }

    vecData_t.erase(vecData_t.end()-3, vecData_t.end());
    vector<uint8> package_crc = CRC_16(vecData_t, static_cast<uint32>(vecData_t.size()));
    vector<uint8> calc_crc;
    calc_crc.push_back(vecData.at(vecData.size()-3));
    calc_crc.push_back(vecData.at(vecData.size()-2));

    if(package_crc != calc_crc)
    {
        qDebug() << __LINE__ << __FUNCTION__ << "协议校验失败:CRC错误";
        qDebug("CRC data:%02X %02X != %02X %02X ",package_crc.at(0), package_crc.at(1), calc_crc.at(0), calc_crc.at(1));
        return false;
    }

    return true;
}

/************************************************************************
* 函数功能: 转译数据
* lpData: 要转译的数据
* len: 数据长度
* bSendState: 1发送，0接收
*************************************************************************/
vector<uint8> CommonPacketObj::unEscapeComProtocol(vector<uint8> &lpData, uint32 len, uint32 bSendState)
{
    //qDebug() << __LINE__ << __FUNCTION__;
    QByteArray t_lpData;
    vector<uint8> verData;
    lpData.resize(len+1);

    t_lpData[0]=TW_PROT_HEADER; //设置帧头
    uint32 iIndex = 1;  //处理数据帧其它区域

    for(quint32 i=1;i<(len-1);i++)
    {
        if(bSendState==0)//接收数据
        {

            if(TW_PROT_HEAND_CONVERT_CHAR1 == lpData[i] && TW_PROT_HEAND_CONVERT_CHAR2 == lpData[i+1])
            {
                t_lpData[iIndex++] = TW_PROT_HEADER;
                i++;
            }
            else if(TW_PROT_HEAND_CONVERT_CHAR1 == lpData[i] && TW_PROT_CONVERT_CHAR == lpData[i+1])
            {
                t_lpData[iIndex++] = TW_PROT_HEAND_CONVERT_CHAR1;
                i++;
            }
            else if(TW_PROT_END_CONVERT_CHAR1 == lpData[i] && TW_PROT_END_CONVERT_CHAR2 == lpData[i+1])
            {
                t_lpData[iIndex++] = TW_PROT_END;
                i++;
            }
            else if(TW_PROT_END_CONVERT_CHAR1 == lpData[i] && TW_PROT_CONVERT_CHAR == lpData[i+1])
            {
                t_lpData[iIndex++] = TW_PROT_END_CONVERT_CHAR1;
                i++;
            }
            else
            {
                t_lpData[iIndex++] = lpData[i];
            }
        }
        else		//发送数据
        {
            if(TW_PROT_HEADER == lpData[i])
            {
                t_lpData[iIndex++]=TW_PROT_HEAND_CONVERT_CHAR1;
                t_lpData[iIndex++]=TW_PROT_HEAND_CONVERT_CHAR2;
            }
            else if(TW_PROT_HEAND_CONVERT_CHAR1 == lpData[i])
            {
                t_lpData[iIndex++]=TW_PROT_HEAND_CONVERT_CHAR1;
                t_lpData[iIndex++]=TW_PROT_CONVERT_CHAR;
            }
            else if(TW_PROT_END == lpData[i])
            {
                t_lpData[iIndex++]=TW_PROT_END_CONVERT_CHAR1;
                t_lpData[iIndex++]=TW_PROT_END_CONVERT_CHAR2;
            }
            else if(TW_PROT_END_CONVERT_CHAR1 == lpData[i])
            {
                t_lpData[iIndex++]=TW_PROT_END_CONVERT_CHAR1;
                t_lpData[iIndex++]=TW_PROT_CONVERT_CHAR;
            }
            else
            {
                t_lpData[iIndex++]=lpData[i];
            }
        }
    }
    //设置帧尾
    t_lpData[iIndex++] = TW_PROT_END;

    verData.insert(verData.end(), t_lpData.begin(), t_lpData.end());
    return verData;
}

/************************************************************************
* 函数功能: 打包数据
* actionType: 一级指令
* objectData: 要打包的数据
*************************************************************************/
vector<uint8> CommonPacketObj::PackBytes(DevFunctionType1 actionType, uint8 objectType, vector<uint8>& objectData)
{
    vector<uint8> object;
    //Data[0]:包头
    object.push_back(0xDF);
    //Data[1～2]:数据长度
    uint8 objLen1 = (objectData.size() >> 8) & 0xFF;
    uint8 objLen2 = objectData.size() & 0xFF;
    object.push_back(objLen1);
    object.push_back(objLen2);
    //Data[5～6]:本帧编号
    object.push_back(0x00);
    object.push_back(0x00);
    //Data[5～6]:一级&二级指令
    object.push_back(static_cast<uint8>(actionType));
    object.push_back(static_cast<uint8>(objectType));
    //Data[7～len]:数据
    if(objectData.size() != 0)
    {
        object.insert(object.end(),objectData.begin(),objectData.end());
    }
    //校验码
    vector<uint8> crc = CRC_16(object, static_cast<uint32>(object.size()));
    object.push_back(crc[0]);
    object.push_back(crc[1]);
    //包尾
    object.push_back(0xFD);

    vector<uint8> ObjectBytes = unEscapeComProtocol(object, static_cast<uint32>(object.size()), 1);
    return ObjectBytes;
}

/************************************************************************
* 函数功能: 打包数据
* actionType: 一级指令
* msg: 要打包的数据
* len: 数据长度
*************************************************************************/
vector<uint8> CommonPacketObj::PackBytes(uint8 actionType, uint8 objectType, char *msg, int len)
{
    //qDebug() << __LINE__ << __FUNCTION__;
    //vector<uint8> bytes = VectorUTF8ToGBK(msg, len);
    vector<uint8> bytes;
    bytes.insert(bytes.end(), msg, msg+len);

    vector<uint8> object;
    //Data[0]:包头
    object.push_back(0xDF);
    //Data[1～2]:数据长度
    uint8 objLen1 = uint8(bytes.size()/256);
    uint8 objLen2 = uint8(bytes.size()%256);
    object.push_back(objLen1);
    object.push_back(objLen2);
    //Data[5～6]:本帧编号
    object.push_back(0x00);
    object.push_back(0x00);
    //Data[5～6]:一级&二级指令
    object.push_back(actionType);
    object.push_back(objectType);
    //Data[7～len]:数据
    if(bytes.size() != 0)
    {
        object.insert(object.end(),bytes.begin(),bytes.end());
    }

    //校验码
    vector<uint8> crc = CRC_16(object, static_cast<uint32>(object.size()));
    object.push_back(crc[0]);
    object.push_back(crc[1]);

    //包尾
    object.push_back(0xFD);
    vector<uint8> ObjectBytes = unEscapeComProtocol(object, static_cast<uint32>(object.size()), 1);
    return ObjectBytes;
}

//解包
vector<vector<uint8>> CommonPacketObj::UnPackBytes(vector<uint8>& vecSource)
{
    vector<vector<uint8>> completePackets;

    while(!vecSource.empty())
    {
        vector<uint8>::iterator beg_it = std::find(vecSource.begin(), vecSource.end(), TW_PROT_HEADER);
        if(beg_it != vecSource.begin())
        {
            //丢弃不完整的数据
            vector<uint8> tmp;
            tmp.insert(tmp.end(), vecSource.begin(), beg_it);
            vecSource.erase(vecSource.begin(), beg_it);
            continue;
        }
        vector<uint8>::iterator end_it = std::find(beg_it, vecSource.end(), TW_PROT_END);
        if(end_it == vecSource.end())
        {
            break;
        }

        //2.跳过不完整的数据（4字节为包头+指令+检验码+包尾）
        if(vecSource.size() < 10)
        {
            break;
        }

        vector<uint8> one_data;
        std::advance(end_it, 1);
        one_data.insert(one_data.end(), vecSource.begin(), end_it);
        vecSource.erase(vecSource.begin(), end_it);

        //转译
        vector<uint8> vecRealData = unEscapeComProtocol(one_data, static_cast<uint32>(one_data.size()), 0);
        //校验正确性
        if(CheckPackage(vecRealData))
        {
            completePackets.push_back(vecRealData);
        }
        else
        {
            qDebug() << __LINE__ << __FUNCTION__ << "CRC校验错误";
        }
        break;
    }
    return completePackets;
}
