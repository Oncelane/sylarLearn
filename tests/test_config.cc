#include "../sylar/config.h"
#include "../sylar/log.h"

sylar::ConfigVar<int>::ptr g_int_value_confg =
    sylar::Config::Lookup("system.port", (int)8080,"system port");

sylar::ConfigVar<float>::ptr g_float_value_confg =
    sylar::Config::Lookup("system.value", (float)14.2f,"system port");

sylar::ConfigVar<std::vector<int> >::ptr g_int_vec_value_confg =
    sylar::Config::Lookup("system.int_vec", std::vector<int>{1,2},"system port");


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
    YAML::Node node = YAML::LoadFile("/home/oncelane/workspace/sylar/bin/conf/log.yml");
    print_yaml(node, 0);
    // SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << root;
}

void test_config() {
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) <<"before:" << g_int_value_confg->getValue();
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) <<"before:" << g_float_value_confg->toString();
    auto v = g_int_vec_value_confg->getValue();
    for(auto& i : v) {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "befor:" << "int_vec:" << i;
    }

    YAML::Node root = YAML::LoadFile("/home/oncelane/workspace/sylar/bin/conf/log.yml");
    sylar::Config::LoadFromYaml(root);

    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) <<"after:" << g_int_value_confg->getValue();
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) <<"after:" << g_int_value_confg->toString();
    
    v = g_int_vec_value_confg->getValue();
    for(auto& i : v) {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "after:" << "int_vec:" << i;
    }
}
int main(int argc, char** argv) {
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << g_int_value_confg->getValue();
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << g_int_value_confg->toString();

    // test_yaml();
    test_config();
    return 0;
}