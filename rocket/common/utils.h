#ifndef ROCKET_COMMON_UTILS_H
#define ROCKET_COMMON_UTILS_H

#include  <sys/types.h>
#include <unistd.h>

namespace rocket{

pid_t getPid();
pid_t gerThreadId();

}

#endif
