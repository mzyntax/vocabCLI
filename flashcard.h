#pragma once
#include <time.h>

typedef enum {
    EASY,
    GOOD,
    HARD,
    AGAIN,
} Rating; 

typedef enum {
    LEARNING,
    REVIEW,
    RELEARNING,
} State;

typedef struct {
    int index;
    TimeCard last_review;
    State state;
    Rating rating; // How hard is it for the user?
    float stability; // Time based constant for memory decay
    time_t due;
    char *spanish_word;
    char *english_word;
} Flashcard ;

typedef struct {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
} TimeCard ;

// R = exp(ln(0.9) * elapsed_days / S)

// S -> stability controls how fast the forgetting curve drops
