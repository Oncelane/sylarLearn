#include "../sylar/sylar.h"
#include <stdint.h>

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

int count = 0;
// sylar::RWMutex s_mutex;
sylar::Mutex s_mutex;
void fun1() {
    SYLAR_LOG_INFO(g_logger) << "name: "<< sylar::Thread::GetName()
        << " this.name: " << sylar::Thread::GetThis()->getName()
        << " id:" << sylar::GetThreadId()
        << " this.id" << sylar::Thread::GetThis()->getId();
    for(int i = 0; i < 100000; ++i) {
        // sylar::RWMutex::WriteLock lock(s_mutex);
        sylar::Mutex::Lock lock(s_mutex);
        ++count;
    }
}

void fun2() {
    int count=0;
    while(true&&count++<100){
        SYLAR_LOG_INFO(g_logger) << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    }
    
}

void fun3(){
    int count=0;
    while(true&&count++<100){
        SYLAR_LOG_INFO(g_logger) << "====================================================";
    }
}

int main(int argc, char** args) {
    SYLAR_LOG_INFO(g_logger) << "thread test begin";

    YAML::Node node = YAML::LoadFile("/home/oncelane/workspace/sylar/bin/conf/test_log.yml");
    sylar::Config::LoadFromYaml(node);
    std::vector<sylar::Thread::ptr> thrs;
    for(int i = 0; i < 2; ++i) {
        sylar::Thread::ptr thr1(new sylar::Thread(&fun2, "name_" + std::to_string(i*2)));
        sylar::Thread::ptr thr2(new sylar::Thread(&fun3, "name_" + std::to_string(i*2+1)));
        thrs.push_back(thr1);
        thrs.push_back(thr2);
    }

    for(size_t i = 0; i < thrs.size(); ++i) {
        thrs[i]->join();
    }
    SYLAR_LOG_INFO(g_logger) << "thread test end";
    SYLAR_LOG_INFO(g_logger) << "count=" << count;
    return 0;
}