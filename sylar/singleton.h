//单例模式封装
#ifndef __SYLAR_SINGLE_H__
#define __SYLAR_SINGLE_H__
#include <memory>

namespace sylar {

namespace {

//封装了两个单例模板
//因为是模板写法，所以写法虽然是饿汉模式，但实际使用的时候可能会表现出饱汉模式

//饿汉模式
template<class T, class X, int N>
T& GetInstanceX() {
    static T v;
    return v;
}    

template<class T, class X, int N>
std::shared_ptr<T> GetInstancePtr() {
    static std::shared_ptr<T> v(new T);
    return v;
}


template<class T, class X = void, int N = 0>
class Singleton {
public:

    static T* GetInstance() {
        static T v;
        return &v; 
    }
};

template<class T, class X = void, int N = 0>
class SingletonPtr {
public:
    /**
     * @brief 返回单例智能指针
     */
    static std::shared_ptr<T> GetInstance() {
        static std::shared_ptr<T> v(new T);
        return v;
        //return GetInstancePtr<T, X, N>();
    }
};




} // namespace




}

#endif