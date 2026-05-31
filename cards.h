#pragma once
#include <time.h>

typedef enum {
    NA,
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
    time_t last_review;
    State state;
    Rating rating; // How hard is it for the user?
    float stability; // Time based constant for memory decay
    char spanish_word[32];
    char english_word[32];
} Flashcard ;


void log_review_date(Flashcard *card);