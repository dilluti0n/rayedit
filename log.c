#include <stdarg.h>
#include <stdio.h>

#include "log.h"

static enum log_level log_curr = RED_LOG_INFO;
static FILE *log_stream = NULL;

void log_init(enum log_level level, FILE *stream) {
	log_curr = level;
	log_stream = stream;
}

void log_printf(enum log_level level, const char *format, ...) {
	if (level < log_curr)
		return;

	if (log_stream == NULL)
		log_stream = stdout;

	const char *level_str = (level == RED_LOG_DEBUG) ? "DEBUG" :
				(level == RED_LOG_INFO) ? "INFO" :
				(level == RED_LOG_WARNING) ? "WARN" :
				(level == RED_LOG_ERROR) ? "ERROR" :
				(level == RED_LOG_CRITICAL) ? "CRIT" : "UNKNOWN";

	va_list args;
	va_start(args, format);
	fprintf(log_stream, "[%s] ", level_str);
	vfprintf(log_stream, format, args);
	va_end(args);
}
