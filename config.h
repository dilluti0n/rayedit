#pragma once

#ifdef DEBUG

#include <assert.h>
#define ASSERT(cond) assert(cond)
#define LOGLEVEL RED_LOG_DEBUG
#else

#define ASSERT(cond) ((void)0)
#define LOGLEVEL RED_LOG_INFO

#endif
