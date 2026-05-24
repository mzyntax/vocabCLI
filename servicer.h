#pragma once

#include "flashcard.h"

typedef enum {
    CORRECT_GUESS,
    INCORRECT_GUESS,
    INTERNAL_ERROR,
} ScoreOutcome;

typedef enum {
    ENGLISH_WORD, // 0
    SPANISH_WORD, // 1
} EditType;


int edit_flashcard_attribute(Flashcard *card, int edit, void *ptr);

int log_reviewed_card(Flashcard *card, int difficulty);

int submit_flashcard_data(char *en_word, char *es_word);

int shuffle_flashcard (Flashcard *card);

// ScoreOutcome score_english_translation(Flashcard *card, char *en_guess);

void servicer_init ();