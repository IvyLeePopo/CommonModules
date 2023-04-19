#include "Managerbase.h"
#include <string.h>
#include "json/json.h"
#include "GeneralHelp.h"
#include "basedefine.h"
#include "GeneralDefine.h"
#include "TcpClientParse.h"

#define DELETEOBJECT(_object) \
    if(NULL != _object) \
    {\
        delete _object; \
        _object = NULL; \
    }

ManagerBase::ManagerBase()
{
}

ManagerBase::~ManagerBase()
{
    DELETEOBJECT(m_pReqGlobalLaneInfo);
}

void ManagerBase::init()
{

}

void ManagerBase::cycle(void* pObject)
{
    
}