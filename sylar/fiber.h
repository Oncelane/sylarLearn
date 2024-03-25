#pragma once

#include <memory>
#include <functional>
#include <ucontext.h>


namespace sylar {

class Scheduler;

class Fiber : public std::enable_shared_from_this<Fiber>{
friend Scheduler;
public:
    // std::shared_ptr<Fiber> ptr;
    typedef std::shared_ptr<Fiber> ptr;
    enum State {
        INIT, //初始化
        HOLD, //暂停
        EXEC, //执行
        TERM, //结束
        READY, //可执行
        EXCEPT //异常
    }; 

private:
    Fiber();

public:
    Fiber(std::function<void()> cb, size_t stacksize = 0, bool use_caller = false);
    ~Fiber();
    //重置协程函数，重置状态
    //INIT, TERM
    void reset(std::function<void()> cb);
    //切换到当前协程执行
    void swapIn();
    //切换到后台执行
    void swapOut();

    void call();
    void back();

    State getState() const {return m_state;}

    uint64_t getId() const {return m_id;}
public:
    //设置当前协程
    static void SetThis(Fiber* f);
    //返回当前协程
    static Fiber::ptr GetThis();
    //协程切换到后台，设置为Ready状态
    static void YieldToReady();
    //写成切换到后台，设置为Hold状态
    static void YieldToHold();
    //总协程数
    static uint64_t TotalFibers();
    
    static void MainFunc();
    static void CallerMainFunc();
    static uint64_t GetFiberId();
private:
    uint64_t m_id = 0;
    uint32_t m_stacksize = 0;
    State m_state = INIT;

    ucontext_t m_ctx;
    void * m_stack = nullptr;

    std::function<void()> m_cb;

};


}
