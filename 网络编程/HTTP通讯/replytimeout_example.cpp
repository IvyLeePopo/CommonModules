#include "replytimeout.h"
/**
	实例演示：
*/
void example()
{
	ReplyTimeout *replyTimeout = new ReplyTimeout(reply, this, HTP_Finish | HTP_Read | HTP_Error);
	if (m_bExist || m_bPause)
	{
		return 0;
	}
	QVariant variant = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	int nStatusCode = variant.toInt();
	if (replyTimeout->isTimeOut())
	{
		LOG(INFO) << "download file time out Status Code : " << nStatusCode;
	}
}
