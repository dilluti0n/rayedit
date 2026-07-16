#include <stdarg.h>
#include <stdio.h>

#include "log.h"

static enum log_level log_curr = RED_LOG_DEBUG;
static FILE *log_stream = NULL;

void log_init(enum log_level level, FILE *stream) {
	log_curr = level;
	log_stream = stream;
}

const char *map[] = {
	[RED_LOG_INFO] = "INFO",
	[RED_LOG_ERROR] = "ERROR",
	[RED_LOG_DEBUG] = "DEBUG",
	[RED_LOG_WARNING] = "WARNING"
};

void log_printf(enum log_level level, const char *format, ...) {
	if (level < log_curr || level > RED_LOG_ERROR)
		return;

	if (log_stream == NULL)
		log_stream = stdout;

	const char *level_str = map[level];

	va_list args;
	va_start(args, format);
	fprintf(log_stream, "[%s] ", level_str);
	vfprintf(log_stream, format, args);
	va_end(args);
}
