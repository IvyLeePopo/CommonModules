#ifndef MYGLOBAL_H
#define MYGLOBAL_H


#include <list>


using namespace std;




template<typename T>

class JsonArray{

public:

    std::list<T> arr;
    JsonArray(){}
    ~JsonArray(){}
};


#define   JSON_RETRUN_OK   0
#define   JSON_RETURN_ERROR  -1


















#endif // MYGLOBAL_H
