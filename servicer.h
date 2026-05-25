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

void servicer_init ();

int create_timecard(Timecard *card);

int log_reviewed_card(Flashcard *card, int difficulty);

int submit_flashcard_data(char *en_word, char *es_word);

int edit_flashcard_attribute(Flashcard *card, int edit, void *ptr);

int process_flashcards();

// ScoreOutcome score_english_translation(Flashcard *card, char *en_guess);