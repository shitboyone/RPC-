#ifndef ROCKET_COMMON_UTILS_H
#define ROCKET_COMMON_UTILS_H

#include  <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>

namespace rocket{

pid_t getPid();

pid_t getThreadId();

int64_t getNowMs();

int32_t getInt32FromNetByte(const char* buf);
}

#endif
