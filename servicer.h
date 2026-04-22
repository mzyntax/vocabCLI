#pragma once

#include "flashcard.h"

typedef enum {
    CORRECT_GUESS,
    INCORRECT_GUESS,
    INTERNAL_ERROR,
} ScoreOutcome;

int edit_english_word(char *en_edit, int index);

int edit_spanish_word(char *es_edit, int index);

int edit_familiarity_score(int fam_edit, int index);

int edit_total_attempts(int att_edit, int index);

int edit_correct_attempts(int cor_edit, int index);

int submit_flashcard_data(char *en_word, char *es_word);

int shuffle_flashcard (Flashcard *card);

ScoreOutcome score_english_translation(Flashcard *card, char *en_guess);