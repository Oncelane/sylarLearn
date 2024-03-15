#pragma once
#include <sys/syscall.h>
#include <sched.h>
#include <unistd.h>
#include <pthread.h>

namespace sylar {

pid_t GetThreadId();
// uint32_t GetFiberId();
}