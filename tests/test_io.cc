#include "../sylar/sylar.h"
#include "../sylar/iomanager.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

int fd = 0;

void test_fiber() {
    SYLAR_LOG_INFO(g_logger) << "test_fiber";
    fd = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(fd, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "192.168.134.128", &addr.sin_addr.s_addr);

    if(!connect(fd, (const sockaddr *)&addr, sizeof(addr))) {

    } else if(errno == EINPROGRESS) {
        SYLAR_LOG_INFO(g_logger) << "add event error = "<< errno << " " << strerror(errno);
        sylar::IOManager::GetThis()->addEvent(fd, sylar::IOManager::READ, [](){
            SYLAR_LOG_INFO(g_logger) << "read callback";
        });
        sylar::IOManager::GetThis()->addEvent(fd, sylar::IOManager::WRITE, [](){
            SYLAR_LOG_INFO(g_logger) << "write callback";
            // sylar::IOManager::GetThis()->addEvent(fd, sylar::IOManager::READ);
            close(fd);
        });
    } else {
        SYLAR_LOG_INFO(g_logger) << "else " << errno << " " << strerror(errno);
    }
    

}

int sock = 0;

void test_fiber2() {
    SYLAR_LOG_INFO(g_logger) << "test_fiber";

    sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "112.80.248.75", &addr.sin_addr.s_addr);

    if (!connect(sock, (const sockaddr*)&addr, sizeof(addr))) {
    } else if(errno == EINPROGRESS) {
        SYLAR_LOG_INFO(g_logger) << "add event errno=" << errno << " " << strerror(errno);
        sylar::IOManager::GetThis()->addEvent(sock, sylar::IOManager::WRITE, [](){
            SYLAR_LOG_INFO(g_logger) << "write callback";
            int rt = write(sock, "GET / HTTP/1.1\r\ncontent-length: 0\r\n\r\n",38);
            SYLAR_LOG_INFO(g_logger) << "write rt = " << rt;
            });
        sylar::IOManager::GetThis()->addEvent(sock, sylar::IOManager::READ, [](){
            SYLAR_LOG_INFO(g_logger) << "read callback";
            char temp[1000];
            int rt = read(sock, temp, 1000);
            if (rt >= 0) {
                std::string ans(temp, rt);
                SYLAR_LOG_INFO(g_logger) << "read:["<< ans << "]";
            } else {
                SYLAR_LOG_INFO(g_logger) << "read rt = " << rt;
            }
            });

    } else {
        SYLAR_LOG_INFO(g_logger) << "else " << errno << " " << strerror(errno);
    }
}

void test01() {
    sylar::IOManager iom(2, true, "IOM");
    iom.schedule(test_fiber2);
}


void test1() {
    sylar::IOManager iom; 
    iom.schedule(&test_fiber);
}

int main() {
    // g_logger->setLevel(sylar::LogLevel::INFO);
    test01();
    return 0;
}