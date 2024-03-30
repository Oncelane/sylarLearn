#include "../sylar/timer.h" 
#include "../sylar/log.h"
#include "../sylar/iomanager.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();
sylar::Timer::ptr s_timer;
void test_timer() {
    sylar::IOManager iom(1, false, "IOM2");
    s_timer = iom.addTimer(300, []() {
        static int i = 0;
        SYLAR_LOG_INFO(g_logger) << "hello timer i = " << i;
        if (++i == 5) {
            s_timer->reset(300, true);
        }
        if (i == 10) {
            s_timer->cancel();
        }
    }, true);
}

int main(int argc, char** argv) {
    g_logger->setLevel(sylar::LogLevel::INFO);
    SYLAR_LOG_INFO(g_logger) <<"start time test";
    test_timer();
    SYLAR_LOG_INFO(g_logger) <<"end time test";
    return 0;
}
