#include "fiber.h"
#include "config.h"
#include "macro.h"
#include "log.h"
#include "scheduler.h"
#include <atomic>

namespace sylar {

// static bool DEBUG = false;

// static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");
sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

static std::atomic<uint64_t> s_fiber_id {0};
static std::atomic<uint64_t> s_fiber_count {0};

//当前正在执行的协程
static thread_local Fiber* t_fiber = nullptr;
//主协程
static thread_local Fiber::ptr t_threadFiber = nullptr; //master 协程

static ConfigVar<uint32_t>::ptr g_fiber_stack_size = 
    Config::Lookup<uint32_t>("fiber.stack_size", 1024 * 1024 , "fiber staci size");

class MallocStackAllocator {
public:
    static void* Alloc(size_t size) {
        return malloc(size);
    }

    static void Delloc(void* vp, size_t size) {
        return free(vp);
    }
};

using StackAllocator = MallocStackAllocator;

uint64_t Fiber::GetFiberId() {
    if(t_fiber) {
        return t_fiber->getId();
    }
    return 0;
}

//用于初始化主协程
Fiber::Fiber() {
    m_state = EXEC;
    SetThis(this);

    if(getcontext(&m_ctx)) {
        SYLAR_ASSERT2(false,"getcontext");
    }
    ++s_fiber_count;

    // DPrintfDebug(g_logger, "%s [%d]", "Fiber::Fiber()无参构造" ,m_id);
    // SYLAR_LOG_DEBUG(g_logger) << "Fiber::Fiber()无参构造 id=" << m_id;
}

//需要分配栈空间，传入任务，创建子协程
Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool use_caller )
    :m_id(++s_fiber_id)
    ,m_cb(cb) {
    ++s_fiber_count;
    m_stacksize = stacksize ? stacksize : g_fiber_stack_size->getValue();

    m_stack = StackAllocator::Alloc(m_stacksize);
    if(getcontext(&m_ctx)) {
        SYLAR_ASSERT2(false,"getcontext");
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;


    if(!use_caller) {
        //初始化ucontext结构体
        //MainFunc就是这个协程执行的主任务的，在其中执行了外部传入的m_cb()      
        makecontext(&m_ctx, &Fiber::MainFunc, 0);
    } else {
        makecontext(&m_ctx, &Fiber::CallerMainFunc, 0);
    }

    

    // DPrintfDebug(g_logger, "%s [%d]", "Fiber::Fiber(cb)含参构造", m_id);
    // SYLAR_LOG_DEBUG(g_logger) << "Fiber::Fiber(cb)含参构造 id=" << m_id;
}
Fiber::~Fiber() {
    --s_fiber_count;
    if(m_stack) { //回收协程栈空间
        SYLAR_ASSERT(m_state == TERM || m_state == INIT || m_state == EXCEPT);
        StackAllocator::Delloc(m_stack, m_stacksize);
    } else { //一定是主协程
        SYLAR_ASSERT(!m_cb);
        SYLAR_ASSERT(m_state == EXEC);
        Fiber*cur = t_fiber;
        if(cur == this) {
            SetThis(nullptr);
        }
    }
    // DPrintfDebug(g_logger, "%s [%d]", "Fiber::~Fiber()", m_id);
    // SYLAR_LOG_DEBUG(g_logger) << "Fiber::~Fiber() id=" << m_id;
}
//重置协程函数，重置状态
//INIT, TERM
void Fiber::reset(std::function<void()> cb) {
    SYLAR_ASSERT(m_stack);
    SYLAR_ASSERT(m_state == TERM || m_state == INIT || m_state == EXCEPT);
    m_cb = cb;
    if(getcontext(&m_ctx)) {
        SYLAR_ASSERT2(false,"getcontext");
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = INIT;
}
 
void Fiber::call() {
    SetThis(this);
    m_state = EXEC;
    if (swapcontext(&t_threadFiber->m_ctx, &m_ctx)) {
        SYLAR_ASSERT2(false, "swapcontext");
    }
}

void Fiber::back() {

    SetThis(t_threadFiber.get());
        if(swapcontext(&m_ctx, &t_threadFiber->m_ctx)) {
        SYLAR_ASSERT2(false, "swapcontext");
    }
}

//从主协程切换到当前协程执行
void Fiber::swapIn() {
    SetThis(this);
    SYLAR_ASSERT(m_state != EXEC);
    m_state = EXEC;
    if(swapcontext(&Scheduler::GetMainFiber()->m_ctx, &m_ctx)) {
        SYLAR_ASSERT2(false, "swapcontext");
    }
}


//从当前协程切换到主协程
void Fiber::swapOut() {
    SetThis(Scheduler::GetMainFiber());
    if(swapcontext(&m_ctx, &Scheduler::GetMainFiber()->m_ctx)) {
        SYLAR_ASSERT2(false, "swapcontext");
    }
}

//设置当前协程
void Fiber::SetThis(Fiber* f) {
    t_fiber = f;
}

//返回当前协程
//没有子协程的话返回，创建并返回主协程
Fiber::ptr Fiber::GetThis() {
    if(t_fiber) {
        return t_fiber->shared_from_this();
    }
    //无参构造函数，因此创建了主协程
    Fiber::ptr main_fiber(new Fiber);
    SYLAR_ASSERT(t_fiber == main_fiber.get());
    //初始化线程静态变量为主线程
    t_threadFiber = main_fiber;
    return t_fiber->shared_from_this();
}

//子协程切换为主线程，设置为Ready状态
void Fiber::YieldToReady() {
    Fiber::ptr cur = GetThis();
    cur->m_state = READY;
    cur->swapOut();
}

//子协程切换为主线程，设置为Hold状态
void Fiber::YieldToHold() {
    Fiber::ptr cur = GetThis();
    cur->m_state = HOLD;
    cur->swapOut();
}

//总协程数
uint64_t Fiber::TotalFibers() {
    return s_fiber_count;
}

void Fiber::MainFunc() {
    //获得当前协程
    Fiber::ptr cur = GetThis();
    SYLAR_ASSERT(cur);
    try {
        //执行协程任务
        cur->m_cb();
        cur->m_cb = nullptr;
        //协程结束
        cur->m_state = TERM;
    } catch (std::exception& ex) {
        cur->m_state = EXEC;
        SYLAR_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what()
                << std::endl
                << sylar::BacktraceToString();
        
    } catch (...) {
        cur->m_state = EXEC;
        SYLAR_LOG_ERROR(g_logger) << "Fiber Except" 
                << std::endl
                << sylar::BacktraceToString();
    }
    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->swapOut();
    SYLAR_ASSERT2(false, "Fiber should never reach here");
}

void Fiber::CallerMainFunc() {
    //获得当前协程
    Fiber::ptr cur = GetThis();
    SYLAR_ASSERT(cur);
    try {
        //执行协程任务
        cur->m_cb();
        cur->m_cb = nullptr;
        //协程结束
        cur->m_state = TERM;
    } catch (std::exception& ex) {
        cur->m_state = EXEC;
        SYLAR_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what()
                << std::endl
                << sylar::BacktraceToString();
        
    } catch (...) {
        cur->m_state = EXEC;
        SYLAR_LOG_ERROR(g_logger) << "Fiber Except" 
                << std::endl
                << sylar::BacktraceToString();
    }
    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->back();
    SYLAR_ASSERT2(false, "Fiber should never reach here");
}

}