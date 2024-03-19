#include "../sylar/config.h"
#include "../sylar/log.h"
#include <iostream>


sylar::ConfigVar<int>::ptr g_int_value_confg =
    sylar::Config::Lookup("system.port", (int)8080,"system port");

sylar::ConfigVar<float>::ptr g_float_value_confg =
    sylar::Config::Lookup("system.value", (float)14.2f,"system port");

sylar::ConfigVar<std::vector<int> >::ptr g_int_vec_value_confg =
    sylar::Config::Lookup("system.int_vec", std::vector<int>{1,2},"system int vec");

sylar::ConfigVar<std::list<int> >::ptr g_int_list_value_confg =
    sylar::Config::Lookup("system.int_list", std::list<int>{1,2},"system int list");

sylar::ConfigVar<std::set<int> >::ptr g_int_set_value_confg =
    sylar::Config::Lookup("system.int_set", std::set<int>{1,2},"system int set");

sylar::ConfigVar<std::unordered_set<int> >::ptr g_int_uset_value_confg =
    sylar::Config::Lookup("system.int_uset", std::unordered_set<int>{1,2},"system int uset");

sylar::ConfigVar<std::map<std::string, int> >::ptr g_int_map_value_confg =
    sylar::Config::Lookup("system.str_int_map", std::map<std::string, int>{{"China",1},{"Ameriacn",2}},"system string int map");

sylar::ConfigVar<std::unordered_map<std::string, int> >::ptr g_int_umap_value_confg =
    sylar::Config::Lookup("system.str_int_umap", std::unordered_map<std::string, int>{{"China2",1},{"Ameriacn2",2}},"system string int umap");

// sylar::ConfigVar<std::unordered_map<std::string, Person> >::ptr g_person_map =
//     sylar::Config::Lookup("system.person_map", std::unordered_map<std::string, int>{{"China",1},{"Ameriacn",2}},"system string int umap");


void print_yaml(const YAML::Node& node, int level) {
     if(node.IsScalar()) {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level * 4, ' ') <<node.Scalar() << " - " << node.Type() << " - " << level;
     } else if(node.IsNull()) {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level * 4, ' ') << "NULL - " <<node.Type() << " - " << level;
     } else if(node.IsMap()) {
        for(auto it = node.begin();
                it != node.end(); it++) {
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT())<< std::string(level * 4, ' ') << it->first << " - " << it->second.Type() << " - " <<level;
            print_yaml(it->second, level + 1);
        }
     } else if(node.IsSequence()) {
        for(size_t i = 0; i < node.size(); ++i) {
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level * 4, ' ') << i << " - " << node[i].Type() << " - " << level;
            print_yaml(node[i], level + 1);
        }
     }
}


void test_yaml() {
    YAML::Node node = YAML::LoadFile("/home/oncelane/workspace/sylar/bin/conf/test.yml");
    print_yaml(node, 0);
    // SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << root;
}

void test_config() {
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) <<"before:" << g_int_value_confg->getValue();
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) <<"before:" << g_float_value_confg->toString();


#define XX(g_val, name, prefix) \
    {\
        auto& v = g_val->getValue();\
        for(auto& i : v) {\
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix " " #name  ":" << i;\
        }\
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix " " #name  "yaml:" << g_val->toString();\
    }

#define XX_M(g_val, name, prefix) \
{\
    auto& v = g_val->getValue();\
    for(auto& i : v) {\
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix " " #name  ": {" << i.first <<  " - " << i.second << "}";\
    }\
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix " " #name  "yaml:" << g_val->toString();\
} 
    XX(g_int_vec_value_confg, int_vec, before);
    XX(g_int_list_value_confg, int_list, before);
    XX(g_int_set_value_confg, int_set, before);
    XX(g_int_uset_value_confg, int_uset, before);
    XX_M(g_int_map_value_confg, int_int_map, before);
    XX_M(g_int_umap_value_confg, int_int_umap, before);

    YAML::Node root = YAML::LoadFile("/home/oncelane/workspace/sylar/bin/conf/test.yml");
    sylar::Config::LoadFromYaml(root);

    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) <<"after:" << g_int_value_confg->getValue();
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) <<"after:" << g_int_value_confg->toString();


    XX(g_int_vec_value_confg, int_vec, after);
    XX(g_int_list_value_confg, int_list, after);
    XX(g_int_set_value_confg, int_set, after);
    XX(g_int_uset_value_confg, int_uset, after);
    XX_M(g_int_map_value_confg, int_map, after);
    XX_M(g_int_umap_value_confg, int_umap, after);


#undef XX
#undef XX_M

}

class Person {
public:
    Person(){}
    std::string m_name;
    int m_age =0;
    bool m_sex =0;

    std::string toString() const {
        std::stringstream ss;
        ss << "[Person name =" << m_name
            << " age=" << m_age
            << " sex=" << m_sex
            << "]";
        return ss.str();
    }

    bool operator== (const Person& oth) const{
        return m_name == oth.m_name && m_age == oth.m_age && m_sex == oth.m_sex;
    }
};

namespace sylar {
//---------自定义模板--------
template<>
class LexicalCast<std::string, Person > {
public:
    Person operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        Person p;
        p.m_name = node["name"].as<std::string>();
        p.m_age  = node["age"].as<int>();
        p.m_sex = node["sex"].as<bool>();
        return p; 
    }
};

template<>
class LexicalCast<Person, std::string> {
public:
    std::string operator() (const Person & p) {
        YAML::Node node;
        node["name"] =  p.m_name;
        node["age"] = p.m_age;
        node["sex"] = p.m_sex;
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};



}


sylar::ConfigVar<Person>::ptr g_person = sylar::Config::Lookup("class.person", Person(), "system person");



void test_class() {
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << g_person->getValue().toString() << " - " << g_person->toString();

#define XX_PM(g_var, prefix) \
{\
    auto m = g_person_map->getValue();\
    for(auto& i : m) {\
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix ": {" << i.first <<  " - " << i.second.toString() << "}";\
    }\
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix  ": size=" << m.size();\
} 


    g_person->addListener([](const Person& old_value, const Person&new_value ) {
         SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "old_value =" << old_value.toString()
                << " new_value=" << new_value.toString();
    });

    // XX_PM(g_person_map, "calss.map before");
    YAML::Node root = YAML::LoadFile("/home/oncelane/workspace/sylar/bin/conf/test.yml");
    sylar::Config::LoadFromYaml(root);
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << g_person->getValue().toString() << " - " << g_person->toString();
    // XX_PM(g_person_map, "calss.map after");
}


void test_log() {

    std::cout << sylar::LoggerMgr::GetInstance()->toYamlString() << std::endl;

    YAML::Node root = YAML::LoadFile("/home/oncelane/workspace/sylar/bin/conf/log.yml");
    sylar::Config::LoadFromYaml(root);
    std::cout << "======" << std::endl;
    std::stringstream ss;
    ss << root;
    std::cout << ss.str() << std::endl;
    std::cout << "======" << std::endl;
    std::cout << sylar::LoggerMgr::GetInstance()->toYamlString() << std::endl;
}

int main(int argc, char** argv) {


    // test_yaml();
    // test_config();
    // test_class();
    test_log();
        sylar::Config::Visit([](sylar::ConfigVarBase::ptr var) {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "name=" << var->getName()
                << " description=" << var->getDescription()
                << " typedename=" << var->getTypeName()
                << " value=" << var->toString();
    });
    return 0;
}