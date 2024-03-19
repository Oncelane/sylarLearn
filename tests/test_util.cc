#include "../sylar/sylar.h"
#include <assert.h>

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void test_assert(){
    SYLAR_LOG_INFO(g_logger) << sylar::BacktraceToString(10);
    SYLAR_ASSERT2(0 == 1, "hello assert");
}

int main(int argc, char** args) {
    test_assert();
    return 0;
}
