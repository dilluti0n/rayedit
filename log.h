#pragma once

typedef enum {
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR,
	LOG_CRITICAL
} log_level;

void log_set_level(log_level level);
void log_printf(log_level level, const char *format, ...);
