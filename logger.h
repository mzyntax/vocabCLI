#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>

typedef struct {
    FILE *fp;
    char filename[30];
    void *udata;
    int level;
    int id;
} Origin;

enum {TRACE, INFO, DEBUG, WARN, ERROR};

#define log_trace(...) log_to_file (TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...) log_to_file (INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_to_file (DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...) log_to_file (WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_to_file (ERROR, __FILE__, __LINE__, __VA_ARGS__)

#define set_log_file(...) mark_file(__FILE__, __VA_ARGS__)

int mark_file(const char *file, const char *output, ...);

int log_to_file(int level, const char *file, int line, const char *msg, ...);

const char *state_label(int state);

const char *rating_label(int rating);
