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
    State state;
    Rating rating;
    float retention;
    float stability;
    time_t last_review;
    int next_review;
    int correct_tally;
    int incorrect_tally;
    char spanish_word[32];
    char english_word[32];
} Flashcard ;


void card_init ();

void free_heap_cards();

Flashcard *create_card_skeleton ();