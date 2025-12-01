#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

void log_die() {
	exit(EXIT_FAILURE);
}

static const char* basename(const char* path) {
	const char* s = strrchr(path, '/');
	if (!s)
		s = strrchr(path, '\\');
	return s ? s + 1 : path;
}

void __log(TraceLogLevel level, int linum, const char* file, const char* fmt, ...) {
	static char buf[512] = "";

	va_list args = {0};
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	TraceLog(level, "(%s:%d) %s", basename(file), linum, buf);

	extern int exitcode;
	if (level == LOG_FATAL)
		exitcode = EXIT_FAILURE;
}

static void log_cb(int level, const char* fmt, va_list args) {
	static const char* levels[] = {
		[LOG_TRACE] = "abcd",
		[LOG_DEBUG] = "poop",
		[LOG_INFO] = "INFO",
		[LOG_WARNING] = "WARN",
		[LOG_ERROR] = "DAMN",
		[LOG_FATAL] = "FUCK",
	};

	static char buf[512] = "";
	vsnprintf(buf, sizeof(buf), fmt, args);
	fprintf(stderr, "[%s] %s\n", levels[level], buf);
	fflush(stderr);
}

void log_init() {
	SetTraceLogCallback(log_cb);
	SetTraceLogLevel(LOG_WARNING);
}
