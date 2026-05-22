#pragma once

typedef struct {
    int index;
    float correct_attempts;
    float total_attempts;
    float familiarity;
    float recency;
    char *spanish_word;
    char *english_word;
} Flashcard ;