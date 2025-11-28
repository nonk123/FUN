#pragma once

#include "raylib.h"

void log_init();

/// Log with filename and line number for easier tracing.
///
/// Please use the macros from this header instead of calling this function directly.
void __log(TraceLogLevel level, int linum, const char* file, const char* fmt, ...);
#define _log(level, ...) __log(level, __LINE__, __FILE__, __VA_ARGS__)

#define info(...) _log(LOG_INFO, __VA_ARGS__)
#define warn(...) _log(LOG_WARNING, __VA_ARGS__)
