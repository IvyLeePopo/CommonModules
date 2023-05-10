/*
 * @Description: 的用法
 * @Author: Ivy
 * @Date: 2022-05-10 09:34:43
 * @LastEditTime: 2022-05-10 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */


#pragma once
#include "application/sdloginmanager.h"
 // 这里要保证你的实例在application中初始化时创建，在application销毁前销毁
class DownloadManager;
class UploadManager;
class DataManager;
class FileOperationManager;
class SDThreadManager;
class HttpFunc;
class SdLocalServer;
class SdLocalClient;
class SdImageResourceLoader;
class SdSubApplication;
class SdApplicationBase;
class SdWebViewServer;
class PubGlobalInstances
{
private:
	PubGlobalInstances();
public:
	static void setApplication(SdApplicationBase *application);
	static SdApplicationBase *getApplication();
	static void setLoginManager(SdLoginManager *loginManager);
	static SdLoginManager *getLoginManager();
	static void setDownloadManager(DownloadManager *downloadManager);
	static DownloadManager *getDownloadManager();
	static void setUploadManager(UploadManager *uploadManager);
	static UploadManager *getUploadManager();
	static void setDataManager(DataManager *dataManager);
	static DataManager *getDataManager();
	static void setFileOperationManager(FileOperationManager *fileOperationManager);
	static FileOperationManager *getFileOperationManager();
	static void setThreadManager(SDThreadManager *threadManager);
	static SDThreadManager *getThreadManager();
	static void setHttpFuncManager(HttpFunc *httpFuncManaget);
	static HttpFunc *getHttpFunccManager();
	static void setResourceLoaderInstance(SdImageResourceLoader *resourceLoader);
	static SdImageResourceLoader *getResourceLoaderInstance();
	static void setLocalServer(SdLocalServer *localServer);
	static SdLocalServer *getLocalServer();
	static void setLocalClient(SdLocalClient *localClient);
	static SdLocalClient *getLocalClient();
	static void setWebVewServer(SdWebViewServer *webVewServer);
	static SdWebViewServer *getWebVewServer();
	static QWidget *getMainWidget();
private:
	static SdApplicationBase                *m_application;
	static SdLoginManager                   *m_loginManager;
	static DownloadManager                  *m_downloadManager;
	static UploadManager                    *m_uploadManager;
	static DataManager                      *m_dataManager;
	static FileOperationManager             *m_fileOperationManager;
	static SDThreadManager                  *m_threadManager;
	static HttpFunc                         *m_httpFuncManager;
	static SdImageResourceLoader            *m_imageResourceLoader;
	static SdLocalServer                    *m_localServer;
	static SdLocalClient                    *m_localClient;
	static SdWebViewServer                  *m_webVewServer;
};

#define SdApplicationInstance                   PubGlobalInstances::getApplication()
#define SdLoginManagerInstance                  PubGlobalInstances::getLoginManager()
#define DownloadManagerInstance                 PubGlobalInstances::getDownloadManager()
#define UploadManagerInstance                   PubGlobalInstances::getUploadManager()
#define DataManagerInstance                     PubGlobalInstances::getDataManager()
#define FileOptManagerInstance                  PubGlobalInstances::getFileOperationManager()
#define ThreadManagerInstance                   PubGlobalInstances::getThreadManager()
#define HttpFuncManagetInstance                 PubGlobalInstances::getHttpFunccManager()
#define ResourceLoaderInstance                  PubGlobalInstances::getResourceLoaderInstance()
#define LocalServerInstance                     PubGlobalInstances::getLocalServer()
#define LocalClientInstance                     PubGlobalInstances::getLocalClient()
#define WebViewServerInstance                   PubGlobalInstances::getWebVewServer()
