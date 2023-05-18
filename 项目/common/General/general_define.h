#pragma once
#include <string>
#include <vector>

using namespace std;

// 删除对象，_object 被删除对象、_func 删除的函数
#define DESTROYOBJECT(_object, _func)\
    if(NULL != _object) \
    {                   \
        _func(_object); \
        _object = NULL; \
    }

#define DELETEOBJECT(_object) \
    if(NULL != _object) \
    {\
        delete _object; \
        _object = NULL; \
    }

#define DELETEBUFF(_object) \
    if(NULL != _object) \
    {\
        delete [] _object; \
        _object = NULL; \
    }

// 获取 JSON 值(单字节字符串)
#define GETSTRINGFROMJSON(key, root, des) \
    if(!root[key].isNull() && root[key].isString()) \
    { \
        des = root[key].asString();\
    }else{\
        des = "";\
    }
// 获取 JSON 值(爽字节字符串)
#define GETWSTRINGFROMJSON(key, root, des) \
    if(!root[key].isNull() && root[key].isString()) \
    { \
        des = root[key].asCString(); \
    } else{ \
        des = "";\
    }

// 获取JSON的节点
#define GETNODEFROMJSON(key, root, des) \
    if(!root[key].isNull()) \
    { \
        des = root[key];\
    }
// 获取 Json 值(int)
#define GETINTFROMJSON(key, root, des) \
    if(!root[key].isNull() && (root[key].isInt() || root[key].isUInt())) \
    { \
        des = root[key].asInt(); \
    }else{\
        des = 0;\
    }

// 获取 Json 值(float)
#define GETFLOATFROMJSON(key, root, des) \
    if(!root[key].isNull() && root[key].isDouble()) \
    { \
        des = root[key].asDouble(); \
    }else{\
        des = 0.0;\
    }

// 定义get\set模块
#define GETSET_FUNC(Type, key, Name) \
    private: \
        Type m_##key##Name; \
    public: \
        void set##Name(Type Name) \
        { \
            m_##key##Name = Name; \
        } \
        Type get##Name() \
        { \
            return m_##key##Name; \
        }
