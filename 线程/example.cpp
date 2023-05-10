#include "thread.h"
#include <QJsonObject>
/**
	实例演示：
*/

#define CheckResult(a)  if(!(a)) \
  return;
#define CheckResultWithRet(a, b)  if(!(a)) \
  return (b);

#define OPEN_RENDER_CMD "render.exe"

//用例：启动一个应用exe
void example()
{
	Thread *thread = new Thread("OpenRenderThread");

	thread->setThreadFunc([=] {
		CheckResult(LocalServerInstance);
		QString cmd = OPEN_RENDER_CMD;
		QJsonObject obj;
		int clientId = LocalServerInstance->nextClientId();
		obj.insert("clientId", clientId);
		genericfunc::insertSomeInfoToSubProcJson(obj);
		cmd += QJsonDocument(obj).toJson().toBase64();
		QString renderExePath = Common::getRenderExePath();
		QProcess process;

		if (!process.startDetached(renderExePath, QStringList(cmd)))
		{
			LOG(INFO) << "irender start fail";
		}
	});

	// 启动线程（试着找下这个接口）
	ThreadExecutor::startSdThread(thread);
}

