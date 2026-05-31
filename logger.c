#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define max_ids 10
#define CHECK(x) if (!(x)) {return -1;}

static struct {
    Origin *loggers[max_ids];
    int possible_ids;
} cb_info;

static const char *level_arr[] = {
    "TRACE", "INFO", "DEBUG", "WARNING", "ERROR"
};

int set_logger_id() {
    if (cb_info.possible_ids < max_ids) {
        return cb_info.possible_ids++;
    }
    return -1;
}

int set_log_file(char *filename, char *origin_file) {
    Origin *logger = malloc(sizeof(Origin));
    CHECK(logger);

    int id = set_logger_id();

    if (id == 1) {
        free(logger);
        return -1;
    }

    logger->id = id;
    
    FILE *fptr = fopen(filename, "w+");
    if (fptr != NULL) {
        logger->fp = fptr;
    } else {
        fprintf(stderr, "logger: failed to open '%s'\n", filename);
        free(logger);
        return -1;
    }

    logger->filename = origin_file;

    cb_info.loggers[id] = logger;
    return 0;
}

int log_to_file(int level, const char *file, int line, char *msg, ...){
    va_list ap;
    FILE *fp = NULL;

    for (int i = 0; i < max_ids; i++ ) {
        Origin *logger = cb_info.loggers[i];
        if (logger == NULL) {
            continue;
        }
        if (strcmp(logger->filename, file) == 0) {
            fp = logger->fp;
            break;
        }
    }
    
    CHECK(fp);

    va_start(ap, msg);

    const char *error_level = level_arr[level];

    fprintf(fp, "%s | %d | %s |", 
        file, line, error_level);
    vfprintf(fp, msg, ap);
    fprintf(fp, "\n");

    va_end(ap);
    return 0;
}
    