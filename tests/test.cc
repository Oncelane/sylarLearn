#include <iostream>
#include  "../sylar/log.h"
#include "../sylar/util.h"
int main(int args, char** argv) {
    sylar::Logger::ptr logger(new sylar::Logger);
    logger->addAppender(sylar::LogAppender::ptr(new sylar::StdoutLogAppender));

    sylar::FileLogAppender::ptr file_appender(new sylar::FileLogAppender("./log.txt"));
    logger->addAppender(file_appender);
    
    //默认格式//"%d{%Y-%m-%d %H:%M:%S}%T[%t]%T[%F]%T[%p]%T[%c]%T%f:%l%T%m%n"
    //%m -- 消息体
    //%p -- level
    //%r -- 启动后时间
    //%c -- 日志名称
    //%t -- 线程id
    //%n -- 回车换行
    //%d -- 时间
    //%f -- 文件名
    //%l -- 行号
    //%T -- Tab
    //%F -- 协程id
    SYLAR_LOG_INFO(logger) << "test macro";
    SYLAR_LOG_FMT_WARN(logger,"%s","can this work?");
    
    sylar::LogFormatter::ptr newFormatter(new sylar::LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T[%t]%T[%F]%T[%p]%T[%c]%T%f:%l%T%m%n"));
    file_appender->setFormatter(newFormatter);

    SYLAR_LOG_FMT_DEBUG(logger,"%s test middle %s","i can sure this is work", "testEnd ");
    
    auto xxlog = sylar::LoggerMgr::GetInstance()->getLogger("xx");
    printf("xxlog:%s\n",xxlog->getName().c_str());
    SYLAR_LOG_INFO(xxlog) << "xx say so";
    xxlog->addAppender(sylar::LogAppender::ptr(new sylar::StdoutLogAppender));
    SYLAR_LOG_FMT_WARN(xxlog,"%s","xx say so in fmt");
    return 0;
}