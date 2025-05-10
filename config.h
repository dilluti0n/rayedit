#pragma once

#ifdef DEBUG
#include <stdio.h>
#include <assert.h>
#define ASSERT(cond) assert(cond)
#include <stdlib.h>
#define mem_malloc malloc
#define mem_free free
#else
#define ASSERT(cond) ((void)0)
#endif
