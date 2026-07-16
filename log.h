#pragma once

#include <stdio.h>		/* FILE */

enum log_level {
	RED_LOG_DEBUG = 0,
	RED_LOG_INFO,
	RED_LOG_WARNING,
	RED_LOG_ERROR,		/* fatal; abort the system */
	RED_LOG_NONE,
};

void log_init(enum log_level level, FILE *stream);
void log_printf(enum log_level level, const char *format, ...);

#define ERROR(f, ...) log_printf(RED_LOG_ERROR, (f), __VA_ARGS__)
