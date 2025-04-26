#pragma once

#ifdef DEBUG
#include <assert.h>
#define ASSERT(cond) assert(cond)
#else
#define ASSERT(cond) ((void)0)
#endif
