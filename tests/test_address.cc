#include "sylar/address.h"
#include "sylar/log.h"

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void test() {
    std::vector<sylar::Address::ptr> addrs;

    SYLAR_LOG_INFO(g_logger) << "begin";

    bool v = sylar::Address::Lookup(addrs, "localhost:3080");
    SYLAR_LOG_INFO(g_logger) << "end";
    if(!v) {
        SYLAR_LOG_ERROR(g_logger) << "lookup fail";
        return;
    }

    for(size_t i = 0; i < addrs.size(); ++i) {
        SYLAR_LOG_INFO(g_logger) << i << " - " << addrs[i]->toString();
    }

    auto addr = sylar::Address::LookupAny("localhost:4080");
    if(addr) {
        SYLAR_LOG_INFO(g_logger) << *addr;
    } else {
        SYLAR_LOG_ERROR(g_logger) << "error";
    }
}

void test_ipv4() {
    auto addr = sylar::IPAddress::Create("127.0.0.8");
    if(addr) {
        SYLAR_LOG_INFO(g_logger) << addr->toString();
    }

}

void test_iface() {
    std::multimap<std::string, std::pair<sylar::Address::ptr, uint32_t> > results;
    bool v = sylar::Address::GetInterfaceAddresses(results);
    if(!v) {
        SYLAR_LOG_ERROR(g_logger) << "Getinterfaceaddress  fail";
        return ;
    }
    for(auto &i : results) {
        SYLAR_LOG_INFO(g_logger) << i.first << " - " << i.second.first->toString() << " - "
            << i.second.second;
    }
}

int main () {
    // test();
    // test_ipv4();
    test_iface();
    return 0;
}