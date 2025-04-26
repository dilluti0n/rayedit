#pragma once

#ifdef DEBUG
#include <assert.h>
#define ASSERT(cond) assert(cond)
#include <stdlib.h>
#define MemAlloc malloc
#define MemFree free
#else
#define ASSERT(cond) ((void)0)
#endif
