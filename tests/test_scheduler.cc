#include "../sylar/scheduler.h" 
#include "../sylar/log.h"

static sylar::Logger::ptr g_logger  = SYLAR_LOG_ROOT();

int count = 0;
sylar::Mutex mu;

void test_fiber(int id) {
    for(int i = 0; i < 10000000; ++i){
       
        if(i == 5000000){
            SYLAR_LOG_INFO(g_logger) << "test "<<"id:"<< id <<" in fiber" <<"count:"<<count;
            SYLAR_LOG_INFO(g_logger) << "misson :"<<id<<" yield";
            sylar::Fiber::YieldToReady();
            SYLAR_LOG_INFO(g_logger) << "misson :"<<id<<" resume";
        }
         sylar::Mutex::Lock lock(mu);
        count++;
    }
    SYLAR_LOG_INFO(g_logger) << "test "<<"id:"<< id <<" in fiber" <<"count:"<<count;

    // if(--count >= 0) {
    //     sylar::Scheduler::GetThis()->schedule(&test_fiber);
    //     sylar::Scheduler::GetThis()->schedule(&test_fiber);
    // }
}

void test_fiber_one() {
    SYLAR_LOG_INFO(g_logger) << "test in fiber";

    sleep(1);

    // sylar::Scheduler::GetThis()->schedule(&test_fiber);
    
}

int main(int argc, char **argv) {
    SYLAR_LOG_INFO(g_logger) << "main start";
    sylar::Scheduler sc(2, false, "test");
    sc.start();
    sc.schedule(std::bind(&test_fiber, 1));
    sc.schedule(std::bind(&test_fiber, 2));
    sc.schedule(std::bind(&test_fiber, 3));
    sc.schedule(std::bind(&test_fiber, 4));
    sc.schedule(std::bind(&test_fiber, 5));
    sc.stop();
    SYLAR_LOG_INFO(g_logger) << "main end";
    return 0;
}
