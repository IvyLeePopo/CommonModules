
#include "pch.h"
#include "application/pubglobalinstances.h"
#include "datamanager/datamanager.h"
#include "datatransfermgr/downloadmgr/downloadmanager.h"
#include "datatransfermgr/uploadmgr/uploadmanager.h"
#include "localsocket/sdlocalserver.h"
#include "urlmanager/httpfunc.h"
#include "web/sdwebviewsever.h"

SdApplicationBase *PubGlobalInstances::m_application = nullptr;
SdLoginManager *PubGlobalInstances::m_loginManager = nullptr;
DownloadManager *PubGlobalInstances::m_downloadManager = nullptr;
UploadManager *PubGlobalInstances::m_uploadManager = nullptr;
DataManager *PubGlobalInstances::m_dataManager = nullptr;
FileOperationManager *PubGlobalInstances::m_fileOperationManager = nullptr;
SDThreadManager *PubGlobalInstances::m_threadManager = nullptr;
HttpFunc *PubGlobalInstances::m_httpFuncManager = nullptr;
SdLocalServer *PubGlobalInstances::m_localServer = nullptr;
SdLocalClient *PubGlobalInstances::m_localClient = nullptr;
SdImageResourceLoader *PubGlobalInstances::m_imageResourceLoader = nullptr;
SdWebViewServer *PubGlobalInstances::m_webVewServer = nullptr;

void PubGlobalInstances::setApplication(SdApplicationBase *application)
{
	m_application = application;
}
SdApplicationBase *SdApplicationInstance
{
	return m_application;
}
void PubGlobalInstances::setLoginManager(SdLoginManager *loginManager)
{
	m_loginManager = loginManager;
}
SdLoginManager *SdLoginManagerInstance
{
	return m_loginManager;
}
void PubGlobalInstances::setDownloadManager(DownloadManager *downloadManager)
{
	m_downloadManager = downloadManager;
}
DownloadManager *DownloadManagerInstance
{
	return m_downloadManager;
}
void PubGlobalInstances::setUploadManager(UploadManager *uploadManager)
{
	m_uploadManager = uploadManager;
}
UploadManager *UploadManagerInstance
{
	return m_uploadManager;
}
void PubGlobalInstances::setDataManager(DataManager *dataManager)
{
	m_dataManager = dataManager;
}
DataManager *DataManagerInstance
{
	return m_dataManager;
}
void PubGlobalInstances::setFileOperationManager(FileOperationManager *fileOperationManager)
{
	m_fileOperationManager = fileOperationManager;
}
FileOperationManager *FileOptManagerInstance
{
	return m_fileOperationManager;
}
void PubGlobalInstances::setThreadManager(SDThreadManager *threadManager)
{
	m_threadManager = threadManager;
}
SDThreadManager *ThreadManagerInstance
{
	return m_threadManager;
}
void PubGlobalInstances::setHttpFuncManager(HttpFunc *httpFuncManager)
{
	m_httpFuncManager = httpFuncManager;
}
HttpFunc *HttpFuncManagetInstance
{
	return m_httpFuncManager;
}
void PubGlobalInstances::setResourceLoaderInstance(SdImageResourceLoader *resourceLoader)
{
	m_imageResourceLoader = resourceLoader;
}
SdImageResourceLoader *ResourceLoaderInstance
{
	return m_imageResourceLoader;
}
void PubGlobalInstances::setLocalServer(SdLocalServer *localServer)
{
	m_localServer = localServer;
}
SdLocalServer *LocalServerInstance
{
	return m_localServer;
}
void PubGlobalInstances::setLocalClient(SdLocalClient *localClient)
{
	m_localClient = localClient;
}
SdLocalClient *LocalClientInstance
{
	return m_localClient;
}
void PubGlobalInstances::setWebVewServer(SdWebViewServer *webVewServer)
{
	m_webVewServer = webVewServer;
}
SdWebViewServer *WebViewServerInstance
{
	return m_webVewServer;
}
