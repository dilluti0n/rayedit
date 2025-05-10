#pragma once

#include <stdio.h>		/* FILE */

enum log_level {
	RED_LOG_ALL = 0,
	RED_LOG_DEBUG,
	RED_LOG_INFO,
	RED_LOG_WARNING,
	RED_LOG_ERROR,		/* fatal; abort the system */
	RED_LOG_CRITICAL,	/* more fatal */
	RED_LOG_NONE,		/* do not display log */
};

void log_set_level(enum log_level level);
void log_fprintf(enum log_level level, FILE *fp, const char *format, ...);
