#pragma once

#include <string>
#include <memory>
#include <stdint.h>
#include <list>
#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>
#include <mutex>
#include <map>
#include <functional>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <yaml-cpp/yaml.h>
#include "thread.h"
#include "singleton.h"
#include "util.h"


#define SYLAR_LOG_LEVEL(logger, level) \
    if(logger->getLevel() <= level) \
        sylar::LogEventwrap(sylar::LogEvent::ptr(new sylar::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, sylar::GetThreadId(),\
                233, time(0)))).getSS()

#define SYLAR_LOG_DEBUG(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::DEBUG)
#define SYLAR_LOG_INFO(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::INFO)
#define SYLAR_LOG_WARN(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::WARN)
#define SYLAR_LOG_ERROR(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::ERROR)
#define SYLAR_LOG_FATAL(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::FATAL)

#define SYLAR_LOG_ROOT() sylar::LoggerMgr::GetInstance()->getRoot()
#define SYLAR_LOG_NAME(name) sylar::LoggerMgr::GetInstance() ->getLogger(name)
/**
 * @brief 使用格式化方式将日志级别level的日志写入到logger
 */
#define SYLAR_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->getLevel() <= level) \
        sylar::LogEventwrap(sylar::LogEvent::ptr(new sylar::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, sylar::GetThreadId(),\
                233, time(0)))).getEvent()->format(fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别debug的日志写入到logger
 */
#define SYLAR_LOG_FMT_DEBUG(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::DEBUG, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别info的日志写入到logger
 */
#define SYLAR_LOG_FMT_INFO(logger, fmt, ...)  SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::INFO, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别warn的日志写入到logger
 */
#define SYLAR_LOG_FMT_WARN(logger, fmt, ...)  SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::WARN, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别error的日志写入到logger
 */
#define SYLAR_LOG_FMT_ERROR(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::ERROR, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别fatal的日志写入到logger
 */
#define SYLAR_LOG_FMT_FATAL(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::FATAL, fmt, __VA_ARGS__)

namespace sylar {

class Logger;
class LoggerManager;

//日志类型
class LogLevel {
public:
    enum Level{
        UNKNOW,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
    };

    static const char* ToString (LogLevel::Level level);
    static LogLevel::Level FromString (const std::string& str);
};



//日志事件
class LogEvent {
public:
    typedef std::shared_ptr<LogEvent> ptr;

    LogEvent(std::shared_ptr<Logger> logger,LogLevel::Level level, const std::string& file,int32_t m_line, uint32_t ElapseFormatItem
    , uint32_t thread_id, uint32_t fiber_id, uint64_t time);
    const std::string& getFile() const {return m_file;}


    int32_t getLine() const {return m_line;}
    uint32_t getElapse() const {return m_elapse;}
    uint32_t getThreadId() const { return m_threadId;}
    uint32_t getFiberId() const {return m_fiberId;}
    uint64_t getTime() const {return m_time;}
    std::string getContent() {return m_ss.str( );}
    std::shared_ptr<Logger> getLogger() const { return m_logger;}
    LogLevel::Level getLevel() const {return m_level;}
    std::stringstream& getSS() {return m_ss;}

    void format(const char* fmt, ...);
    void format(const char* fmt, va_list al);
private:
    const std::string m_file = nullptr;   //文件名
    int32_t m_line = 0;             //行号
    uint32_t m_elapse = 0;          //程序启动到现在的毫秒数
    uint32_t m_threadId = 0;        //线程id
    uint32_t m_fiberId = 0;         //协程id
    uint64_t m_time;                //时间戳
    std::stringstream  m_ss;

    std::shared_ptr<Logger> m_logger;
    LogLevel ::Level m_level;
};


class LogEventwrap {
public:
    LogEventwrap(LogEvent::ptr e);
    ~LogEventwrap();
    std::stringstream& getSS();
    LogEvent::ptr getEvent() const {return m_event;}
private:
    LogEvent::ptr m_event;
};






//日志格式化
class  LogFormatter {
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter (const std::string& pattern);

    // 传入日志器，日志等级，日志内容
    // 两种返回方式，string型的，还有流类型的
    std::string format(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event);
    std::ostream& format(std::ostream& ofs,std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event);
    
    //解析日志模板，生成日志解析后的格式
    void init();

    bool isError() const {return m_error;}

    const std::string getPatten() const {return m_pattern;}
public:
    class FormatItem {
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        virtual ~FormatItem() {}
        virtual void format(std::ostream&os, std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event) = 0; 
    };
private:
    //日志格式模板
    std::string m_pattern;
    //解析后的日志格式
    std::vector<FormatItem::ptr> m_items;
    //解析是否有错误
    bool m_error = false;
};
 
//日志输出地
class LogAppender {
friend class Logger;
public:
    typedef std::shared_ptr<LogAppender> ptr;
    typedef Mutex MutexType;
    virtual ~LogAppender() {}

    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    LogFormatter::ptr getFormatter() const { return m_formatter;}

    //修改日志格式器
    void setFormatter(LogFormatter::ptr val);

    //获取日志格式器
    LogFormatter::ptr getFormatter();

    //获取日志等级
    LogLevel::Level getLevel()const {return m_level;}

    //重置日志等级
    void setLevel(LogLevel::Level level) {m_level= level;}

    //日志输出目标的配置转成YAML String
    virtual std::string toYamlString() = 0;

protected:
    //日志等级默认为最低级
    LogLevel::Level m_level = LogLevel::DEBUG;
    //是否有格式器
    bool m_hasFormatter = false;
    //日志格式器
    LogFormatter::ptr m_formatter;
    //
    MutexType m_mutex;
};


//日志器
class Logger :public std::enable_shared_from_this<Logger>{
friend class LoggerManager;
public:
    typedef std::shared_ptr<Logger> ptr;
    typedef Mutex MutexType;
    Logger(const std::string& name = "root");

    //写日志
    void log(LogLevel::Level level,const LogEvent::ptr event);
    //写各种level的日志
    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    //增加日志输出目标
    void addAppender(LogAppender::ptr appender);
    //删除日志输出目标
    void delAppender(LogAppender::ptr appender);
    //清空日志输出目标
    void clearAppenders();


    //修改日志等级
    void setLevel(LogLevel::Level val) {m_level = val;}

    LogLevel::Level getLevel() const { return m_level;}

    //设置日志格式器
    void setFormatter(LogFormatter::ptr val);

    //设置日志格式模板
    void setFormatter(const std::string& val);

    //获取日志格式模板
    LogFormatter::ptr getFormatter();

    const std::string& getName() const {return m_name;}

    /**
     * @brief 将日志器的配置转成YAML String
     */
    std::string toYamlString();

private:
    std::string m_name;                         //日志名称
    LogLevel::Level m_level;                    //日志等级
    std::list<LogAppender::ptr> m_appenders;    //日志输出目标列表
    LogFormatter::ptr m_formatter;              //日志格式器
    Logger::ptr m_root;
    MutexType m_mutex; 
};

//输出到控制台的appender
class StdoutLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
    std::string toYamlString() override;

};



//输出到文件的appender
class FileLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<FileLogAppender> ptr;

    FileLogAppender(const std::string& filename);
    void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
    std::string toYamlString() override;
    //重新打开日志文件
    bool ropen();
private:
    //文件路径
    std::string m_filename;
    //文件流 
    std::ofstream m_filestream;
    //上次重新打开时间
    uint64_t m_lastTime = 0;
};

class LoggerManager {
public:
    typedef Mutex MutexType;
    LoggerManager();

    Logger::ptr getLogger(const std::string& name);

    void init();

    Logger::ptr getRoot() const {return m_root;}

    std::string toYamlString();
    

private:
    MutexType m_mutex;
    Logger::ptr m_root;
    std::map<std::string, Logger::ptr> m_loggers;

};

typedef sylar::Singleton<LoggerManager> LoggerMgr;



}

