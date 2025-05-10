#include <stdarg.h>
#include <stdio.h>

#include "log.h"

static enum log_level log_curr = RED_LOG_INFO;

void log_set_level(enum log_level level) {
	log_curr = level;
}

void log_fprintf(enum log_level level, FILE *fp, const char *format, ...) {
	if (level < log_curr)
		return;

	const char *levelStr = (level == RED_LOG_DEBUG) ? "DEBUG" :
			       (level == RED_LOG_INFO) ? "INFO" :
			       (level == RED_LOG_WARNING) ? "WARN" :
			       (level == RED_LOG_ERROR) ? "ERROR" :
			       (level == RED_LOG_CRITICAL) ? "CRIT" : "UNKNOWN";

	va_list args;
	va_start(args, format);
	fprintf(fp, "[%s] ", levelStr);
	vfprintf(fp, format, args);
	fprintf(fp, "\n");
	va_end(args);
}
