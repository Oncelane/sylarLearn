#pragma once

#include "log.h"
#include "util.h"

#include <memory>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>
#include <string>
#include <map>
#include <set>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <functional>



namespace sylar {

class ConfigVarBase {
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;
    ConfigVarBase( const std::string& name, const std::string& description = "")
        :m_name(name)
        ,m_description(description) {
            std::transform(m_name.begin(), m_name.end(),m_name.begin(), ::tolower);
        }
    virtual ~ConfigVarBase();

    const std::string& getName() const {return m_name;}
    const std::string& getDescription() const {return m_description;}

    virtual std::string toString()=0;
    virtual bool fromString(const std::string& val) =0;
    virtual std::string getTypeName()=0;
protected:
    std::string m_name;
    std::string m_description;

};

//F from_type, T to_type
template<class F,class T>
class LexicalCast {
public:
    T operator()(const F& v) {
        return boost::lexical_cast<T>(v);
    }
};

//---------vector--------
template<class T>
class LexicalCast<std::string, std::vector<T> > {
public:
    std::vector<T> operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::vector<T> vec;
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>() (ss.str()));
        }
        return vec;
    }
};

template<class T>
class LexicalCast<std::vector<T>, std::string> {
public:
    std::string operator() (const std::vector<T> & v) {
        YAML::Node node;
        for(auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

//---------list--------
template<class T>
class LexicalCast<std::string, std::list<T> > {
public:
    std::list<T> operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::list<T> vec;
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>() (ss.str()));
        }
        return vec;
    }
};

template<class T>
class LexicalCast<std::list<T>, std::string> {
public:
    std::string operator() (const std::list<T> & v) {
        YAML::Node node;
        for(auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

//---------set--------
template<class T>
class LexicalCast<std::string, std::set<T> > {
public:
    std::set<T> operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::set<T> vec;
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>() (ss.str()));
        }
        return vec;
    }
};

template<class T>
class LexicalCast<std::set<T>, std::string> {
public:
    std::string operator() (const std::set<T> & v) {
        YAML::Node node;
        for(auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

//---------unordered_set--------
template<class T>
class LexicalCast<std::string, std::unordered_set<T> > {
public:
    std::unordered_set<T> operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::unordered_set<T> vec;
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>() (ss.str()));
        }
        return vec;
    }
};

template<class T>
class LexicalCast<std::unordered_set<T>, std::string> {
public:
    std::string operator() (const std::unordered_set<T> & v) {
        YAML::Node node;
        for(auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

//---------map--------
template<class T>
class LexicalCast<std::string, std::map<std::string, T> > {
public:
    std::map<std::string, T> operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::map<std::string, T> vec;
        std::stringstream ss;
        for(auto it = node.begin(); it != node.end(); ++it) {
            ss.str("");
            ss << it->second;
            vec.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
        }
        return vec;
    }
};

template<class T>
class LexicalCast<std::map<std::string, T>, std::string> {
public:
    std::string operator() (const std::map<std::string, T> & v) {
        YAML::Node node;
        for(auto& i : v) {
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));

        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

//---------umap--------
template<class T>
class LexicalCast<std::string, std::unordered_map<std::string, T> > {
public:
    std::unordered_map<std::string, T> operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::unordered_map<std::string, T> vec;
        std::stringstream ss;
        for(auto it = node.begin(); it != node.end(); ++it) {
            ss.str("");
            ss << it->second;
            vec.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
        }
        return vec;
    }
};

template<class T>
class LexicalCast<std::unordered_map<std::string, T>, std::string> {
public:
    std::string operator() (const std::unordered_map<std::string, T> & v) {
        YAML::Node node;
        for(auto& i : v) {
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));

        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

struct LogAppenderDefine {
    int type = 0; //1 File, 2 Stdout
    LogLevel::Level level = LogLevel::UNKNOW;
    std::string formatter;
    std::string file;

    bool operator==(const LogAppenderDefine& oth) const {
        return type == oth.type
            && level == oth.level
            && formatter == oth.formatter
            && file == oth.file;
    }
};

struct LogDefine {
    std::string name;
    LogLevel::Level level = LogLevel::UNKNOW;
    std::string formatter;
    std::vector<LogAppenderDefine> appenders;

    bool operator==(const LogDefine& oth) const {
        return name == oth.name
            && level == oth.level
            && formatter == oth.formatter
            && appenders == appenders;
    }

    bool operator<(const LogDefine& oth) const {
        return name < oth.name;
    }

    bool isValid() const {
        return !name.empty();
    }
};

//------------

template<>
class LexicalCast<std::string, LogDefine> {
public:
    LogDefine operator()(const std::string& v) {
        YAML::Node n = YAML::Load(v);
        LogDefine ld;
        if(!n["name"].IsDefined()) {
            std::cout << "log config error: name is null, " << n
                      << std::endl;
            throw std::logic_error("log config name is null");
        }
        ld.name = n["name"].as<std::string>();
        ld.level = LogLevel::FromString(n["level"].IsDefined() ? n["level"].as<std::string>() : "");
        if(n["formatter"].IsDefined()) {
            ld.formatter = n["formatter"].as<std::string>();
        }

        if(n["appenders"].IsDefined()) {
            //std::cout << "==" << ld.name << " = " << n["appenders"].size() << std::endl;
            for(size_t x = 0; x < n["appenders"].size(); ++x) {
                auto a = n["appenders"][x];
                if(!a["type"].IsDefined()) {
                    std::cout << "log config error: appender type is null, " << a
                              << std::endl;
                    continue;
                }
                std::string type = a["type"].as<std::string>();
                LogAppenderDefine lad;
                if(type == "FileLogAppender") {
                    lad.type = 1;
                    if(!a["file"].IsDefined()) {
                        std::cout << "log config error: fileappender file is null, " << a
                              << std::endl;
                        continue;
                    }
                    lad.file = a["file"].as<std::string>();
                    if(a["formatter"].IsDefined()) {
                        lad.formatter = a["formatter"].as<std::string>();
                    }
                } else if(type == "StdoutLogAppender") {
                    lad.type = 2;
                    if(a["formatter"].IsDefined()) {
                        lad.formatter = a["formatter"].as<std::string>();
                    }
                } else {
                    std::cout << "log config error: appender type is invalid, " << a
                              << std::endl;
                    continue;
                }

                ld.appenders.push_back(lad);
            }
        }
        return ld;
    }
};

template<>
class LexicalCast<LogDefine, std::string> {
public:
    std::string operator()(const LogDefine& i) {
        YAML::Node n;
        n["name"] = i.name;
        if(i.level != LogLevel::UNKNOW) {
            n["level"] = LogLevel::ToString(i.level);
        }
        if(!i.formatter.empty()) {
            n["formatter"] = i.formatter;
        }

        for(auto& a : i.appenders) {
            YAML::Node na;
            if(a.type == 1) {
                na["type"] = "FileLogAppender";
                na["file"] = a.file;
            } else if(a.type == 2) {
                na["type"] = "StdoutLogAppender";
            }
            if(a.level != LogLevel::UNKNOW) {
                na["level"] = LogLevel::ToString(a.level);
            }

            if(!a.formatter.empty()) {
                na["formatter"] = a.formatter;
            }

            n["appenders"].push_back(na);
        }
        std::stringstream ss;
        ss << n;
        return ss.str();
    }
};



//FromStr T operator(const std::string&)
//ToStr std::string operator()(const T&)
template<class T, class FromStr = LexicalCast<std::string,T>, class ToStr = LexicalCast<T, std::string> >
class ConfigVar : public ConfigVarBase {
public:
    typedef std::shared_ptr<ConfigVar> ptr;
    typedef std::function<void(const T& old_value, const T& new_value)> on_change_cb;

    ConfigVar(const std::string& name,
            const T& default_value
            ,const std::string& description ="")
        :ConfigVarBase(name, description)
        ,m_val(default_value) {
    }

    std::string toString() override {
        try{
            // return boost::lexical_cast<std::string>(m_val);
            return ToStr()(m_val);
        }catch (std::exception& e) {
            SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar::toString execption"
                << e.what() << " convert " << typeid(m_val).name() << " to stiring"; 
        }
            return "";
    }

    bool fromString(const std::string& val) override {
        try{
            // m_val = boost::lexical_cast<T>(val);
            setValue(FromStr()(val));
        }catch (std::exception& e) {
            SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar::fromString execption"
                << e.what() << " convert string to" << typeid(m_val).name() << " to stiring"; 
        }
            return false;
    }

    const T getValue() const { return m_val;}

    void setValue(const T& v) {
        if(v == m_val) {
            return;
        }

        for(auto& i:m_cbs){
            i.second(m_val,v);
        }
        m_val = v;
    }

    virtual std::string getTypeName()override { return typeid(T).name();}
    void addListener(uint64_t key, on_change_cb cb) {
        m_cbs[key] = cb;
    }
    void delListener(uint64_t key) {
        m_cbs.erase(key);
    }
    on_change_cb getListener(uint64_t key) {
        auto it = m_cbs.find(key);
        return it == m_cbs.end() ? nullptr : it->second;
    }
    void clearListener(){
        m_cbs.clear();
    }
private:
    T m_val;
    //变更回调函数组，key 要求唯一，可以用hash
    std::map<uint64_t, on_change_cb> m_cbs;
};

class Config {
public:
    typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;

    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name,
        const T& default_value, const std::string& description = "") {
            auto it = GetDatas().find(name);
            if(it != GetDatas().end()) {
                auto tmp = std::dynamic_pointer_cast<ConfigVar<T> >(it->second);\
                if(tmp) {
                    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "Lookup name =" << name << " exists";
                    return tmp;
                } else {
                    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "Lookup name =" << name << " exists but type not"
                        << typeid(T).name() << "real type="<< it->second->getTypeName() << it->second->toString();
                    return nullptr;
                }
            }


            if(name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789")
                != std::string::npos) {
                    SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "Lookup name invalid" << name;
                    throw std::invalid_argument(name);
            }

            typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
            GetDatas()[name] = v;
            return v; 
        }

    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name) {
        auto it = GetDatas().find(name);
        if(it == GetDatas().end()) {
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
    }
    static void LoadFromYaml(const YAML::Node& node);

    static ConfigVarBase::ptr LookupBase(const std::string& name);
private:
    static ConfigVarMap& GetDatas() {
        static ConfigVarMap s_datas;
        return s_datas;
    }
};

sylar::ConfigVar<std::set<LogDefine> >::ptr g_log_defines =
    sylar::Config::Lookup("logs", std::set<LogDefine>(), "logs config");

struct LogIniter {
    LogIniter() {
        g_log_defines->addListener(1513,[](const std::set<LogDefine>& old_value,
                    const std::set<LogDefine>& new_value){
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "on_logger_conf_changed";
            for(auto& i : new_value) {
                auto it = old_value.find(i);
                sylar::Logger::ptr logger;
                if(it == old_value.end()) {
                    //新增logger
                    logger = SYLAR_LOG_NAME(i.name);
                } else {
                    if(!(i == *it)) {
                        //修改的logger
                        logger = SYLAR_LOG_NAME(i.name);
                    } else {
                        continue;
                    }
                }
                logger->setLevel(i.level);
                //std::cout << "** " << i.name << " level=" << i.level
                //<< "  " << logger << std::endl;
                if(!i.formatter.empty()) {
                    logger->setFormatter(i.formatter);
                }

                logger->clearAppenders();
                for(auto& a : i.appenders) {
                    sylar::LogAppender::ptr ap;
                    if(a.type == 1) {
                        ap.reset(new FileLogAppender(a.file));
                    } else if(a.type == 2) {
                        // if(!sylar::EnvMgr::GetInstance()->has("d")) {
                        //     ap.reset(new StdoutLogAppender);
                        // } else {
                        //     continue;
                        // }
                        //
                        ap.reset(new StdoutLogAppender);
                        //
                    }
                    ap->setLevel(a.level);
                    if(!a.formatter.empty()) {
                        LogFormatter::ptr fmt(new LogFormatter(a.formatter));
                        if(!fmt->isError()) {
                            ap->setFormatter(fmt);
                        } else {
                            std::cout << "log.name=" << i.name << " appender type=" << a.type
                                      << " formatter=" << a.formatter << " is invalid" << std::endl;
                        }
                    }
                    logger->addAppender(ap);
                }
            }

            for(auto& i : old_value) {
                auto it = new_value.find(i);
                if(it == new_value.end()) {
                    //删除logger
                    auto logger = SYLAR_LOG_NAME(i.name);
                    logger->setLevel((LogLevel::Level)0);
                    logger->clearAppenders();
                }
            }
        });
    }
};

static sylar::LogIniter __log_init;

}