#ifndef JSON_BASE_H
#define JSON_BASE_H


#include "json.h"

#include "json_base.h"

#include "json_global.h"

#include <list>

using namespace std;

#define WRITE_STRING(name)    (writeString(#name,name))
#define WRITE_INT(name)    (writeInt(#name,name))
#define WRITE_MUTI_CLASS(name)    (writeMutiClass(#name,name))
#define WRITE_SIGNAL_CLASS(name)    (writeSignalClass(#name,name))
#define PARSE_TO_STRING(name) (name=ParseToString(#name))
#define PARSE_TO_INT(name) (name=ParseToInt(#name))
#define PARSE_TO_MUTI_CLASS(value) (ParseToMutiClass(#value,value))
#define PARSE_TO_SIGNAL_CLASS(value) (ParseToSignalClass(#value,value))



class  JSON_BASE
{
public:
    JSON_BASE(){}
    virtual ~JSON_BASE(){}


    void writeString(string strName,string strValue)
    {
        root[strName] = strValue;
    }

    void writeInt(string intName,int intValue)
    {
        root[intName] = intValue;
    }

    template<typename ClassTemp>
    void writeMutiClass(string className,JsonArray<ClassTemp> JsonArrayValue){
        Json::Value classNameRoot;
        Json::Value classNameRootOne;
        typename list<ClassTemp>::iterator it;


        for(it=JsonArrayValue.arr.begin();it!=JsonArrayValue.arr.end();it++){

            JSON_BASE *pHelper = (JSON_BASE *)&*it;
            classNameRootOne = pHelper->ToJson();
            classNameRoot.append(classNameRootOne);
        }

        root[className] = classNameRoot;
    };

    template<typename ClassTemp>

    void writeSignalClass(string className,ClassTemp Value){
        Json::Value classNameRoot;
        JSON_BASE *pHelper = (JSON_BASE *)&Value;
        classNameRoot = pHelper->ToJson();

        root[className] = classNameRoot;
    };

    string WriteRootEnd()
    {
        string m_requestData = writer.write(root);
        return m_requestData;
    }

    Json::Value WriteSubEnd()
    {
        return root;
    };



    string ParseToString(string text)
    {
        string value;value.clear();
        if(Json::stringValue == root[text].type())
        {
            value = root[text].asCString();
        }

        return value;
    }

    int ParseToInt(string text)
    {
        int value=-1;
        if(Json::intValue == root[text].type())
        {
            value = root[text].asInt();
        };

        return value;
    }

    template<typename ClassTemp>

    void  ParseToMutiClass(string text,JsonArray<ClassTemp> &mJsonArray){


        if(Json::arrayValue == root[text].type())
        {
            Json::Value  DataVal = root[text];

            for(int i=0;i<root[text].size();++i){

                Json::Value one = DataVal[i];
                ClassTemp mHelper;
                mHelper.FromJson(one);
                mJsonArray.arr.push_back(mHelper);
            }

        };

        return ;
    }

    template<typename ClassTemp>

     void  ParseToSignalClass(string text,ClassTemp &value){


        if(Json::objectValue == root[text].type())
        {
            Json::Value  DataVal = root[text];

            for(int i=0;i<root[text].size();++i){

                Json::Value one = DataVal[i];
                value.FromJson(one);
            }

        };

        return ;
    }


    ///一级字符串转json
    int parseRootJson(string text)
    {
        if(!reader.parse(text.c_str(),root))
        {
            return  JSON_RETURN_ERROR;
        }

        return JSON_RETRUN_OK;
    }
    ///二级以下json解析
    int parseSubJson(Json::Value root)
    {
        this->root = root;
        return 0;
    }


    Json::Value root;
    Json::FastWriter writer;
    Json::Reader reader;


    virtual Json::Value ToJson()
    {
        return "";
    }


    virtual void FromJson(string data)
    {
        return ;
    }


};

#endif // JSON_BASE_H
