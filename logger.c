#include <stdio.h>
#include <stdarg.h>

static struct {
    FILE *fp;
    void *udata;
    char *filename;
    int level;
} cb_info;

static const char *level_arr[] = {
    "TRACE", "INFO", "DEBUG", "WARNING", "ERROR"
};

void set_log_file(char *filename) {
    cb_info.fp = fopen(filename, "w+");
    cb_info.filename = filename;
}

int log_to_file(int level, const char *file, int line, char *msg, ...){
    va_list ap;
    va_start(ap, msg);

    const char *error_level = level_arr[level];

    fprintf(cb_info.fp, "%s | %d | %s |", 
        file, line, error_level);
    vfprintf(cb_info.fp, msg, ap);
    fprintf(cb_info.fp, "\n");

    va_end(ap);
    return 0;

}
    