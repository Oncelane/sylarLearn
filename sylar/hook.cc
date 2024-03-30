#include "hook.h"
#include "fiber.h"
#include "iomanager.h"
#include <functional>
#include <dlfcn.h>

namespace sylar{

static thread_local bool t_hook_enble = false;

#define HOOK_FUN(XX) \
    XX(sleep) \
    XX(usleep) \
    XX(nanosleep) \
    XX(socket) \
    XX(connect) \
    XX(accept) \
    XX(read) \
    XX(readv) \
    XX(recv) \
    XX(recvfrom) \
    XX(recvmsg) \
    XX(write) \
    XX(writev) \
    XX(send) \
    XX(sendto) \
    XX(sendmsg) \
    XX(close) \
    XX(fcntl) \
    XX(ioctl) \
    XX(getsockopt) \
    XX(setsockopt)

void hook_init() {
    static bool is_inited = false;
    if(is_inited) {
        return;
    }
#define XX(name) name ## _f = (name ## _fun)dlsym(RTLD_NEXT, #name) ;
    HOOK_FUN(XX);
#undef XX
}

//在程序执行main之前，会对全局变量进行初始化，利用构造函数进行main前的程序执行
struct _HookIniter {
    _HookIniter() {
        hook_init();
    }
};

static _HookIniter s_hock_initer;

bool is_hook_enable() {
    return t_hook_enble;
}
void set_hook_enable(bool flag) {
    t_hook_enble = flag;
}




}

extern "C" {
#define XX(name) name ## _fun name ##_f = nullptr;
    HOOK_FUN(XX);
#undef XX
}



unsigned int sleep(unsigned int seconds) {
    if(!sylar::t_hook_enble) {
        return sleep_f(seconds);
    }
    sylar::Fiber::ptr fiber = sylar::Fiber::GetThis();
    sylar::IOManager* iom = sylar::IOManager::GetThis();
    // iom->addTimer(seconds * 1000, std::bind(&sylar::IOManager::schedule, iom,fiber));
    iom->addTimer(seconds * 1000, [iom, fiber] () {
        iom->schedule(fiber);
    });
    sylar::Fiber::YieldToHold();
    return 0;
}


int usleep(useconds_t usec) {
     if(!sylar::t_hook_enble) {
        return usleep_f(usec);
     }
    sylar::Fiber::ptr fiber = sylar::Fiber::GetThis();
    sylar::IOManager* iom = sylar::IOManager::GetThis();
    // iom->addTimer(usec / 1000, std::bind(&sylar::IOManager::schedule, iom,fiber));
    iom->addTimer(usec / 1000, [iom, fiber](){
        iom->schedule(fiber);
    });
    sylar::Fiber::YieldToHold();
    return 0;
}


int nanpsleep (const struct timespec *req, struct timespec *rem) {
    if(!sylar::t_hook_enble) {
        return nanosleep_f(req,rem);
    }
    int timeout_ms = req->tv_sec * 1000 + req->tv_nsec /1000 /1000;
    sylar::Fiber::ptr fiber = sylar::Fiber::GetThis();
    sylar::IOManager* iom = sylar::IOManager::GetThis();
    iom->addTimer(timeout_ms, [iom, fiber](){
        iom->schedule(fiber);
    });
    sylar::Fiber::YieldToHold();
    return 0;
}