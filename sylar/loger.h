#ifndef __SYLAR_LOG_H__
#define __SYLAR_LOG_H__

#include <string>
#include <memory>
#include <stdint.h>
#include <list>
#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>

namespace sylar {

//日志事件
class LogEvent {
public:
    typedef std::shared_ptr<LogEvent> ptr;

    LogEvent();
    const char * getFile() const ( return m_file;)
    int32_t getLine() const {return m_line;}
    uint32_t getElapse() const {return m_elapse;}
    uint32_t getThreadId() const { return m_threadId;}
private:
    const char *m_file = nullptr;   //文件名
    int32_t m_line = 0;             //行号
    uint32_t m_elapse = 0;          //程序启动到现在的毫秒数
    uint32_t m_threadId = 0;        //线程id
    uint32_t m_fiberId = 0;         //协程id
    uint32_t m_time;                //时间戳
    std::string m_content;
};

//日志类型
class LogLevel {
public:
    enum Level{
        DEBUG = 1,
        INFO,
        WARN,
        ERROR,
        FATAL
    };
};



//日志格式化
class  LogFormatter {
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter (const std::string& pattern);

    std::string format(LogEvent::ptr event);
    void init();
pubilc:
    class FormatItem {
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        virtual ~FormatItem() {}
        virtual void format(std::ostream&os, LogEvent::ptr event) = 0; 
    };
private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;
};

//日志输出地
class LogAppender {
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender() {}

    virtual void log(LogLevel::Level level, LogEvent::ptr event) = 0;
    LogFormatter::ptr getFormatter() const { return m_formatter;}

protected:
    LogLevel::Level m_level;
    LogFormatter::ptr m_formatter;
};


//日志器
class Logger {
public:
    typedef std::shared_ptr<Logger> ptr;

    Logger(const std::string& name = "root");

    void log(LogLevel::Level level,const LogEvent::ptr event);

    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    LogLevel::Level getLevel() const { return m_level;}
    void setLevel(LogLevel::Level val) {m_level = val;}

private:
    std::string m_name;         //日志名称
    LogLevel::Level m_level;    //日志等级
    std::list<LogAppender::ptr> m_appenders;//日志列表
};

//输出到控制台的appender
class StdoutLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    virtual void log(LogLevel::Level level,LogEvent::ptr event) override;
private:

};


//输出到文件的appender
class FileLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<FileLogAppender> ptr;

    FileLogAppender(const std::string& filename);
    virtual void log(LogLevel::Level level,LogEvent::ptr event) override;
    bool ropen();
private:
    std::string m_filename;
    std::ofstream m_filestream;
};


}

#endif