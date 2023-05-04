#ifndef COMMONBUSSOURCE_H
#define COMMONBUSSOURCE_H


#include <map>

class CommonBus;

using namespace std;



typedef map<int,CommonBus* > CommonMap;
typedef pair<int,CommonBus* > PairCommonMap;


class CommonBus;

class CommonBusSource
{
public:
    CommonBusSource();

public:

    static CommonMap myMap;//保存单例模式类
    static bool isNetworkAccessible;//网络是否正常

};

#endif // COMMONBUSSOURCE_H
