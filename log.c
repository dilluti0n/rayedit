#include <stdarg.h>
#include <stdio.h>

#include "log.h"

static log_level log_curr = LOG_INFO;

void log_set_level(log_level level) {
	log_curr = level;
}

void log_fprintf(log_level level, FILE *fp, const char *format, ...) {
	if (level < log_curr)
		return;

	const char *levelStr = (level == LOG_DEBUG) ? "DEBUG" :
                               (level == LOG_INFO) ? "INFO" :
                               (level == LOG_WARNING) ? "WARN" :
                               (level == LOG_ERROR) ? "ERROR" :
                               (level == LOG_CRITICAL) ? "CRIT" : "UNKNOWN";

	va_list args;
	va_start(args, format);
	fprintf(fp, "[%s] ", levelStr);
	vfprintf(fp, format, args);
	fprintf(fp, "\n");
	va_end(args);
}
