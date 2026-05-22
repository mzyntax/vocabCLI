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
    FAMILIARITY, // 2
    RECENCY, // 3
    TOTAL_ATTEMPTS, // 4
    CORRECT_ATTEMPTS, // 5
} EditType;


int edit_flashcard_attribute(Flashcard *card, int edit, void *ptr);

int edit_english_word(char *en_edit, Flashcard *card);

int edit_spanish_word(char *es_edit, Flashcard *card);

int edit_familiarity_score(float fam_edit, Flashcard *card);

int edit_total_attempts(float att_edit, Flashcard *card);

int edit_correct_attempts(float cor_edit, Flashcard *card);

int submit_flashcard_data(char *en_word, char *es_word);

int shuffle_flashcard (Flashcard *card);

// ScoreOutcome score_english_translation(Flashcard *card, char *en_guess);

void servicer_init ();