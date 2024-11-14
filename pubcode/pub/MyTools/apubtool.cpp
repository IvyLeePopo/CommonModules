#include "../pubcode/pub/MyTools/apubtool.h"

#include <time.h>
#include <QDateTime>
#include <QDebug>
#include <QProcess>
#include <QTextCodec>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QFileInfo>
#ifdef Q_OS_WIN
	#include <windows.h>
#else
    #ifdef Q_OS_MAC
        #include <sys/param.h>
        #include <sys/mount.h>
        #include "mach_gettime.h"
    #else
        #include <sys/statfs.h>
    #endif
#endif
//---------------------------------------------------------
APubTool::APubTool()
{

}
//---------------------------------------------------------
#ifdef Q_OS_WIN
quint64 getDiskFreeSpaceWin(QString driver)
{
	LPCWSTR lpcwstrDriver=(LPCWSTR)driver.utf16();

	ULARGE_INTEGER liFreeBytesAvailable, liTotalBytes, liTotalFreeBytes;

	if( !GetDiskFreeSpaceEx( lpcwstrDriver, &liFreeBytesAvailable, &liTotalBytes, &liTotalFreeBytes) )
	{
		qDebug() << "ERROR: Call to GetDiskFreeSpaceEx() failed.";
		return 0;
	}
	return (quint64) liTotalFreeBytes.QuadPart/1024/1024;
}
#else
quint64 getDiskFreeSpaceLinux(QString sDriver)
{
	struct statfs diskInfo;

	statfs(sDriver.toLocal8Bit().data(), &diskInfo);
	unsigned long long blocksize = diskInfo.f_bsize; //每个block里包含的字节数
	unsigned long long totalsize = blocksize * diskInfo.f_blocks; //总的字节数，f_blocks为block的数目
	//printf("Total_size = %llu B = %llu KB = %llu MB = %llu GB\n",
	//	totalsize, totalsize>>10, totalsize>>20, totalsize>>30);

	unsigned long long freeDisk = diskInfo.f_bfree * blocksize; //剩余空间的大小
	unsigned long long availableDisk = diskInfo.f_bavail * blocksize; //可用空间大小
	//printf("Disk_free = %llu MB = %llu GB\nDisk_available = %llu MB = %llu GB\n",
	//	freeDisk>>20, freeDisk>>30, availableDisk>>20, availableDisk>>30);

	return freeDisk>>20;
}
#endif
/*!
 * \fn APubTool::getDiskFreeSpace(QString sDriver)
 * \brief 获取磁盘剩余空间大小
 *
 * 函数功能详细描述:获取磁盘剩余空间大小
 * \param sDriver 盘符
 * \return 剩余空间，单位:MB
 */
quint64 APubTool::getDiskFreeSpace(QString sDriver)
{
	#ifdef Q_OS_WIN
		return getDiskFreeSpaceWin(sDriver);
	#else
		return getDiskFreeSpaceLinux(sDriver);
	#endif
}
//---------------------------------------------------------
/*!
 * \fn APubTool::getTickCount()
 * \brief 获取自系统开机以来的毫秒数（tick）
 *
 * 函数功能详细描述:获取自系统开机以来的毫秒数（tick）
 * \return
 */
quint64 APubTool::getTickCount()
{
#ifdef Q_OS_WIN
	return GetTickCount();
#else
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#endif
}
//---------------------------------------------------------
/*!
 * \fn APubTool::pingIP(QString sIp)
 * \brief PING网络IP
 *
 * 函数功能详细描述:PING网络IP
 * \param sIp 网络IP
 * \return 是否通
 */
bool APubTool::pingIP(QString sIp)
{

#ifdef Q_OS_WIN
	QString sCmdArg = "ping " + sIp + " -n 1 -i 2";  //windows下的格式
	QProcess pingProcess;
	pingProcess.start(sCmdArg);		//执行ping
	pingProcess.waitForFinished(3000);//等待ping完成
	QString  sOutstr=QString::fromLocal8Bit(pingProcess.readAll().data());//获取ping结果
	if(sOutstr.contains(QString::fromLocal8Bit("往返行程的估计时间")))
	{
		return true;
	}
	return false;
#else
	QString sCmdArg = "ping -s 1 -c 1 " + sIp;		//linux平台下的格式
	int iExitCode = QProcess::execute(sCmdArg);

	/*
	if(0 == iExitCode)
	{
		//it's alive
		qDebug() << "shell ping " + sIp + " sucessed!";
	}
	else
	{
		qDebug() << "shell ping " + sIp + " failed!";
	}*/

	return 0 == iExitCode;
#endif
}
//---------------------------------------------------------
QByteArray APubTool::variantMapToJSON(const QVariantMap &stData, QJsonDocument::JsonFormat jsonFormat)
{
	QJsonDocument jdoc(QJsonObject::fromVariantMap(stData));
	return jdoc.toJson(jsonFormat);
}

QByteArray APubTool::variantListToJSON(const QList<QVariant> &qlData, QJsonDocument::JsonFormat jsonFormat)
{
	QJsonDocument jdoc(QJsonArray::fromVariantList(qlData));
	return jdoc.toJson(jsonFormat);
}

bool APubTool::parseJsonToList(const QByteArray &baJson, QList<QVariant> &qlData)
{
	QJsonDocument jdoc = QJsonDocument::fromJson(baJson);
	if(jdoc.isNull())
		return false;

	QJsonArray jArray = jdoc.array();
	qlData = jArray.toVariantList();
	return true;
}
//---------------------------------------------------------
QByteArray APubTool::mapToJSON(const QMap<QString, QString> &stData)
{
	QVariantMap stJsonMap;
	QMap<QString, QString>::const_iterator it;
	for(it=stData.begin();it!=stData.end();++it)
	{
		stJsonMap[it.key()] = it.value();
	}

	return variantMapToJSON(stJsonMap);
}
//---------------------------------------------------------
bool APubTool::parseJsonToMap(const QByteArray &baJson, QVariantMap &stData)
{
    QJsonParseError json_error;
    QJsonDocument jdoc = QJsonDocument::fromJson(baJson,&json_error);
    if(jdoc.isNull() || json_error.error != QJsonParseError::NoError)
		return false;

	QJsonObject jObj = jdoc.object();
	stData = jObj.toVariantMap();
	return true;
}

/*!
 * \brief 查找文件，主要用于查找配置文件
 * \details 查找顺序：\n
 * （1）查找sDirName目录下是否有sFileName,有则直接返回，否则继续查找当前目录\n
 * （2）查找当前目录是否有sFileName,有则直接直接返回，否则继续向上按照（1）、（2）顺序查找，直到根目录
 * \warning sDirName为空时，只查找当前目录和依次查找上级目录
 * \param sDirName 优先查找该目录下的文件
 * \param sFileName 不带路径的文件名
 * \return 若找到文件则返回查找到的文件绝对路径，否则返回空
 */
QString APubTool::searchFile(const QString &sDirName, const QString &sFileName)
{
	if(sFileName.isEmpty())
	{
		return "";
	}
	QDir sSearchDir = QDir::current();
	while(1)
	{
		if(false == sDirName.isEmpty())
		{
			if(sSearchDir.exists(sDirName))
			{
				if(false == sSearchDir.cd(sDirName))
				{
					return "";
				}
				if(sSearchDir.exists(sFileName))
				{
					return sSearchDir.absoluteFilePath(sFileName);
				}
				if(false == sSearchDir.cdUp())
				{
					return "";
				}
			}
		}
		if(sSearchDir.exists(sFileName))
		{
			return sSearchDir.absoluteFilePath(sFileName);
		}
		if(sSearchDir.isRoot())
		{
			break;
		}
		if(false == sSearchDir.cdUp())
		{
			return "";
		}
	}
	return "";
}

QByteArray APubTool::gbkToUtf8(const QByteArray &gbkByteArray)
{
	QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
	QTextCodec* gbk = QTextCodec::codecForName("GBK");

	QString sTmp = gbk->toUnicode(gbkByteArray);
	QByteArray baUtf8 = utf8->fromUnicode(sTmp);
	return baUtf8;
}

QString APubTool::gbkToUtf8(const QString &gbkStr)
{
    QTextCodec* gbk = QTextCodec::codecForName("GBK");

    QString g2u = gbk->toUnicode(gbk->fromUnicode(gbkStr));
    return g2u;
}

QByteArray APubTool::utf8ToGbk(const QByteArray &utf8ByteArray)
{
    QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
    QTextCodec* gbk = QTextCodec::codecForName("GBK");

    QString sTmp = utf8->toUnicode(utf8ByteArray);
    QByteArray baGbk = gbk->fromUnicode(sTmp);
    return baGbk;
}

QString APubTool::utf8ToGbk(const QString &utf8Str)
{
    QTextCodec* gbk = QTextCodec::codecForName("GBK");

    QString utf2gbk = gbk->toUnicode(utf8Str.toLocal8Bit());
    return utf2gbk;
}
//---------------------------------------------------------
QString APubTool::filterChineseChars(const QByteArray &str)
{
	QByteArray tmp=str;
	for(int i=tmp.length()-1;i>=0;--i)
	{
		if (tmp.at(i)&0x80) tmp.remove(i,1);
	}
	return tmp;
}
//---------------------------------------------------------
bool APubTool::matchPlate(const QString &s1, const QString &s2, int iNeedMatchCnt)
{
	if (iNeedMatchCnt==0)
		return true;
	if (s1.length()<iNeedMatchCnt || s2.length()<iNeedMatchCnt)
		return false;

	if (s1.left(iNeedMatchCnt) == s2.left(iNeedMatchCnt))
		return true;

	if(s1.right(iNeedMatchCnt) == s2.right(iNeedMatchCnt))
		return true;

	//下面开始不完全匹配，支持错位
	for(int i=0;i<=s1.length()-iNeedMatchCnt;i++)
	{
		for (int j=0;j<=s2.length()-iNeedMatchCnt;j++)
		{
			int iMatchCount=0;
			for (int k=0;k<s1.length()-i && k<s2.length()-j;++k)
			{
				if (s1[i+k]==s2[j+k])
					++iMatchCount;
			}

			if (iMatchCount>=iNeedMatchCnt)
				return true;
		}
	}

	return false;
}

bool APubTool::matchPlateFilterChineseChars(const QString &s1, const QString &s2, int iNeedMatchCnt)
{
    QString sObuPlate = filterChineseChars(s1.toLocal8Bit());
    QString sRegPlate = filterChineseChars(s2.toLocal8Bit());

    bool bIsMatch = matchPlate(sObuPlate,sRegPlate,iNeedMatchCnt);
//	qDebug()<<"APubTool::matchPlateFilterChineseChars(),sObuPlate:"<<sObuPlate.toUtf8().data()
//			<<",sRegPlate:"<<sRegPlate.toUtf8().data()
//			<<",isMatch:"<<bIsMatch;

    return bIsMatch;
}

bool APubTool::matchPlateFilterChineseCharsEx(const QString &s1, const QString &s2, int iNeedMatchCnt)
{
    if(APubTool::filterChineseChars(s1.toLocal8Bit()).length()>=7
    || APubTool::filterChineseChars(s2.toLocal8Bit()).length()>=7)
        iNeedMatchCnt += 1;

    return APubTool::matchPlateFilterChineseChars(s1,s2,iNeedMatchCnt);
}

//---------------------------------------------------------
/*!
 * \fn APubTool::unixToLocalTime(const QString &sUnixTimeHex)
 * \brief UNIX时间转换为本地时间
 *
 * \param sUnixTimeHex UNIX时间16进制串
 * \return 本地时间串，格式：yyyyMMddhhmmss
 */
QString APubTool::unixToLocalTime(const QString &sUnixTimeHex)
{
	qint64 iMSecs = sUnixTimeHex.toLongLong(nullptr,16) * 1000;
	QDateTime dtThen = QDateTime::fromMSecsSinceEpoch(iMSecs);

	return dtThen.toString("yyyyMMddhhmmss");
}
//---------------------------------------------------------
/*!
 * \fn APubTool::localToUnixTime(const QString &sLocalTimeHex)
 * \brief 本地时间转换为UNIX时间
 *
 * \param sLocalTimeHex 本地时间串，yyyyMMddhhmmss
 * \return UNIT时间16进制串
 */
QString APubTool::localToUnixTime(const QString &sLocalTimeHex)
{
	QDateTime dtThen = QDateTime::fromString(sLocalTimeHex,"yyyyMMddhhmmss");
	unsigned int iSecs = dtThen.toMSecsSinceEpoch()/1000;
	QString sUnixTime = QString::number(iSecs,16).toUpper();

	while(sUnixTime.length()<8)
		sUnixTime = "0" + sUnixTime;

	return sUnixTime;
}
//---------------------------------------------------------
bool APubTool::delDir(const QString &path)
{
	if (path.isEmpty())
		return false;

	QDir dir(path);
	if(!dir.exists())
		return true;

	dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
	QFileInfoList fileList = dir.entryInfoList(); // 获取所有的文件信息
	foreach (QFileInfo file, fileList)
	{	//遍历文件信息
		if (file.isFile())
		{	// 是文件，删除
			file.dir().remove(file.fileName());
		}else
		{	// 递归删除
			delDir(file.absoluteFilePath());
		}
	}
	return dir.rmpath(dir.absolutePath()); // 删除文件夹
}
//---------------------------------------------------------
QDateTime APubTool::getFileModifyTime(const QString &sFileName)
{
	QFileInfo fileInfo(sFileName);
    return fileInfo.lastModified();
}

QString APubTool::getPlateColorText(int iPlateColor)
{
    QString sColorText = QString::fromUtf8("未知");
    switch(iPlateColor)
    {
    case  0: sColorText = QString::fromUtf8("蓝"); break;
    case  1: sColorText = QString::fromUtf8("黄"); break;
    case  2: sColorText = QString::fromUtf8("黑"); break;
    case  3: sColorText = QString::fromUtf8("白"); break;
    case  4: sColorText = QString::fromUtf8("渐绿"); break;
    case  5: sColorText = QString::fromUtf8("黄绿"); break;
    case  6: sColorText = QString::fromUtf8("蓝白"); break;
    case  7: sColorText = QString::fromUtf8("临时"); break;
    case  9: sColorText = QString::fromUtf8("未知"); break;
    case 11: sColorText = QString::fromUtf8("绿"); break;
    case 12: sColorText = QString::fromUtf8("红"); break;
    default:
        break;
    }

    return sColorText;
}

QString APubTool::getPlateColorText(QString sPlateColor)
{
    bool bOk = false;
    int iPlateColor = sPlateColor.toInt(&bOk);
    if(false == bOk)
        return getPlateColorText(9);

    return getPlateColorText(iPlateColor);
}

QString APubTool::genFullPlateText(QString sPlate, int iPlateColor)
{
    if(sPlate.length()<=0
    || QString::fromLocal8Bit("未识别")==sPlate
    || QString::fromLocal8Bit("无车牌")==sPlate)
        return sPlate;

    return getPlateColorText(iPlateColor)+sPlate;
}

QString APubTool::genFullPlateText(QString sPlate, QString sPlateColor)
{
    bool bOk = false;
    int iPlateColor = sPlateColor.toInt(&bOk);
    if(false == bOk)
        return genFullPlateText(sPlate,9);

    return genFullPlateText(sPlate,iPlateColor);
}

QString APubTool::genFullPlateText(QString sVehicleId)
{
    QStringList slPlateInfo = sVehicleId.split("_");
    QString sPlate = slPlateInfo.at(0);
    QString sPlateColor= "9";
    if(slPlateInfo.size()>1)
        sPlateColor = slPlateInfo.at(1);

    return getPlateColorText(sPlateColor)+sPlate;
}

#define CRC_POLYNOM     0x8408
unsigned short APubTool::genCRC16(const unsigned char *crc_data, int len)
{
    int crc = 0xFFFF, cnt, k;
    for(cnt = 0; cnt < len; cnt++)
    {//loop data length
        crc ^= crc_data[cnt];
        for(k = 0; k < 8; k++)
        {//every bit data
            if(crc & 0x1)
            {
                crc = (crc >> 1) ^ CRC_POLYNOM;
            }
            else
            {
                crc = (crc >> 1);
            }
        }
    }
    crc = (~crc) & 0xFFFF;
    return (unsigned short)crc;
}

QByteArray APubTool::ulong2byte(uint64_t data)
{
    QByteArray baData;baData.resize(8);
    baData[0] = (uchar)((0x000000ff & data));
    baData[1] = (uchar)((0x0000ff00 & data)>>8);
    baData[2] = (uchar)((0x00ff0000 & data)>>16);
    baData[3] = (uchar)((0xff000000 & data)>>24);
    data = data>>32;
    baData[4] = (uchar)((0x000000ff & data));
    baData[5] = (uchar)((0x0000ff00 & data)>>8);
    baData[6] = (uchar)((0x00ff0000 & data)>>16);
    baData[7] = (uchar)((0xff000000 & data)>>24);
    return baData;
}

QByteArray APubTool::uint2byte(uint32_t data)
{
    QByteArray baData;baData.resize(4);
    baData[0] = (uchar)((0x000000ff & data));
    baData[1] = (uchar)((0x0000ff00 & data)>>8);
    baData[2] = (uchar)((0x00ff0000 & data)>>16);
    baData[3] = (uchar)((0xff000000 & data)>>24);
    return baData;
}

QByteArray APubTool::ushort2byte(uint16_t data)
{
    QByteArray baData;baData.resize(2);
    baData[0] = (uchar)((0x00ff & data));
    baData[1] = (uchar)((0xff00 & data)>>8);
    return baData;
}

uint64_t APubTool::byte2ulong(QByteArray data)
{
    uint64_t integer=1;
    uint64_t integer1=1;
    if(data.size() != 8)
        return 0;
    integer |= 0x000000ff&(((uchar)data[0]));
    integer |= 0x0000ff00&(((uchar)data[1])<<8);
    integer |= 0x00ff0000&(((uchar)data[2])<<16);
    integer |= 0xff000000&(((uchar)data[3])<<24);

    integer1 |= 0x000000ff&(((uchar)data[4]));
    integer1 |= 0x0000ff00&(((uchar)data[5])<<8);
    integer1 |= 0x00ff0000&(((uchar)data[6])<<16);
    integer1 |= 0xff000000&(((uchar)data[7])<<24);
    integer|=integer1<<32;
    return integer;
}

uint32_t APubTool::byte2uint(QByteArray data)
{
    uint32_t integer = 0;
    if(data.size() != 4)
        return 0;
    integer |= 0x000000ff&(((uchar)data[0]));
    integer |= 0x0000ff00&(((uchar)data[1])<<8);
    integer |= 0x00ff0000&(((uchar)data[2])<<16);
    integer |= 0xff000000&(((uchar)data[3])<<24);
    return integer;
}

uint16_t APubTool::byte2ushort(QByteArray data)
{
    uint16_t integer = 0;
    if(data.size() != 2)
        return 0;
    integer |= 0x000000ff&(((uchar)data[0]));
    integer |= 0x0000ff00&(((uchar)data[1])<<8);
    return integer;
}
//---------------------------------------------------------
bool APubTool::parseJsonToMap(const char *pJson, QVariantMap &stData)
{
	QByteArray baJson(pJson);
	return parseJsonToMap(baJson,stData);
}
//---------------------------------------------------------
QMap<QString, QString> APubTool::convertMap(const QVariantMap &inputVariantMap)
{
	QMap<QString, QString> ret;
	QVariantMap::const_iterator item = inputVariantMap.begin();
	for(;item != inputVariantMap.end();item++)
	{
		ret[item.key().toLower()] = item.value().toString();
	}
	return ret;
}
//---------------------------------------------------------
QVariantMap APubTool::convertMap(const QMap<QString, QString> &inputMap)
{
	QVariantMap ret;
	QMap<QString, QString>::const_iterator item = inputMap.begin();
	for(;item != inputMap.end();item++)
	{
		ret[item.key()] = item.value();
	}
	return ret;
}
//---------------------------------------------------------
//----------------------------------------------------------------------------
/*!
 * \brief 重启计算机
 */
void APubTool::restartPC()
{
#ifdef Q_OS_WIN
    //Windows下关机命令，进程自带弹窗
    QProcess::startDetached("shutdown -r -t 0");
#elif defined Q_OS_LINUX
    QProcess::startDetached("shutdown -r now");
#endif
}
//------------------------------------------------------------------
/*!
 * \brief 关闭计算机
 */
void APubTool::shutdownPC()
{
#ifdef Q_OS_WIN
    QProcess::startDetached("shutdown -s -t 0");
#elif defined Q_OS_LINUX
    QProcess::startDetached("shutdown -h now");
#endif
}

bool APubTool::saveToFile(QString sFileName, const QByteArray &baData)
{
    if(sFileName.length()<=0)
        return false;

    QFile file(sFileName);
    if(file.open(QIODevice::WriteOnly)==false)
        return false;

    file.write(baData);
    file.close();
    return true;
}

QByteArray APubTool::readFile(QString sFileName)
{
    if(sFileName.length()<=0)
        return "";

    QFile file(sFileName);
    if(file.open(QIODevice::ReadOnly)==false)
        return "";

    QByteArray baRead = file.readAll();
    file.close();
    return baRead;
}
//---------------------------------------------------------
/*!
 * \fn APubTool::crc16MudBus
 * \brief 计算CRC16(MudBus)
 * \param puchMsg   ---数据地址
 * \param usDataLen ---数据长度
 * \return 校验值
 * 步骤：
 * (1). 预置 16 位寄存器为十六进制 0xFFFF（即全为 1） ，称此寄存器为 CRC 寄存器；
 * (2). 把第一个 8 位数据与 16 位 CRC 寄存器的低位相异或，把结果放于 CRC 寄存器；
 * (3). 检测相异或后的CRC寄存器的最低位，若最低位为1，CRC寄存器先右移1位，再与多项式A001H进行异或；若为0，则CRC寄存器右移1位，无需与多项式进行异或。
 * (4). 重复步骤 3 ，直到右移 8 次，这样整个 8 位数据全部进行了处理；
 * (5). 重复步骤 2 到步骤 4，进行下一个 8 位数据的处理；
 * (6). 最后得到的 CRC 寄存器即为 CRC 校验码。
 */
uint16_t APubTool::crc16MudBus(unsigned char *puchMsg, unsigned short usDataLen)
{
    uint16_t uCRC = 0xffff;         //CRC寄存器
    for(unsigned short num=0;num<usDataLen;num++)
    {
        uCRC = (*puchMsg++)^uCRC;   //把数据与16位的CRC寄存器的低8位相异或，结果存放于CRC寄存器。
        for(uint8_t x=0;x<8;x++)    //循环8次
        {
            if(uCRC&0x0001)         //判断最低位为：“1”
            {
                uCRC = uCRC>>1;     //先右移
                uCRC = uCRC^0xA001;	//再与0xA001异或
            }
            else                    //判断最低位为：“0”
            {
                uCRC = uCRC>>1;     //右移
            }
        }
    }
    return uCRC;//返回CRC校验值
}
//---------------------------------------------------------
