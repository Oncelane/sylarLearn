#include "../sylar/scheduler.h" 
#include "../sylar/log.h"

static sylar::Logger::ptr g_logger  = SYLAR_LOG_ROOT();

void test_fiber() {
    static int count = 100;
    SYLAR_LOG_INFO(g_logger) << "test in fiber" <<"count:"<<count;

    if(--count >= 0) {
        sylar::Scheduler::GetThis()->schedule(&test_fiber);
    }
}

void test_fiber_one() {
    SYLAR_LOG_INFO(g_logger) << "test in fiber";

    sleep(1);

    // sylar::Scheduler::GetThis()->schedule(&test_fiber);
    
}

int main(int argc, char **argv) {
    SYLAR_LOG_INFO(g_logger) << "main start";
    sylar::Scheduler sc(5, false, "test");
    sc.start();
    sc.schedule(&test_fiber_one);
    sc.stop();
    SYLAR_LOG_INFO(g_logger) << "main end";
    return 0;
}
