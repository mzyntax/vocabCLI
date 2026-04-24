#pragma once

typedef struct {
    int index;
    float correct;
    float attempts;
    float familiarity;
    char spanish_word[20];
    char english_word[20];
} Flashcard ;