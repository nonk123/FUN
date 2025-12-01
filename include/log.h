#pragma once

#include "raylib.h"

void log_init();
__attribute__((noreturn)) void log_die();

/// Log with filename and line number for easier tracing.
///
/// Please use the macros from this header instead of calling this function directly.
void __log(TraceLogLevel level, int linum, const char* file, const char* fmt, ...);
#define _log(level, ...) __log(level, __LINE__, __FILE__, __VA_ARGS__)

#define info(...) _log(LOG_INFO, __VA_ARGS__)
#define warn(...) _log(LOG_WARNING, __VA_ARGS__)

#define expect(expr, ...)                                                                                              \
	do {                                                                                                           \
		if (!(expr))                                                                                           \
			_log(LOG_FATAL, __VA_ARGS__), log_die();                                                       \
	} while (0)
