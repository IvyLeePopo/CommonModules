
#include "pch.h"
#include "networkutil.h"
#include <QNetworkProxy>
#include "common/sdsetting.h"
#include "common/sdsettingconfig.h"

namespace NetworkUtil
{
	void applyHttpProxy()
	{
		SDSetting setting;
		QString proxyStr = setting.value(sdconfig::httpProxyUrl, "").toString();

		if (proxyStr.isEmpty())
			return;

		// 合法格式：127.0.0.1:8888
		int index = proxyStr.indexOf(":");
		if (index == -1)
		{
			LOG(INFO) << __FUNCTION__ << " failed because local proxy url not contain ':'.";
			return;
		}

		QString proxyUrl = proxyStr.left(index);
		QString proxyPort = proxyStr.right(proxyStr.size() - index - 1);
		if (proxyUrl.isEmpty() || proxyPort.isEmpty())
			return;

		QNetworkProxy proxy;
		proxy.setType(QNetworkProxy::HttpProxy);
		proxy.setHostName(proxyUrl);
		proxy.setPort(static_cast<quint16>(proxyPort.toInt()));
		QNetworkProxy::setApplicationProxy(proxy);
	}
}