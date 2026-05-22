#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>


enum {TRACE, INFO, DEBUG, WARN, ERROR};

#define log_trace(...) log_to_file (TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...) log_to_file (INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_to_file (DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...) log_to_file (WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_to_file (ERROR, __FILE__, __LINE__, __VA_ARGS___)

void set_log_file(char *filename);

int log_to_file(int level, const char *file, int line, char *msg, ...);

