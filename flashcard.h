#pragma once

typedef struct {
    int index;
    int correct;
    int attempts;
    int familiarity;
    char spanish_word[20];
    char english_word[20];
} Flashcard ;