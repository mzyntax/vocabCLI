#pragma once
#include "cards.h"
#include "flashcard_queue/queue.h"

typedef enum {
    CORRECT_GUESS,
    INCORRECT_GUESS,
    INTERNAL_ERROR,
} ScoreOutcome;

typedef enum {
    ENGLISH_WORD, // 0
    SPANISH_WORD, // 1
} EditType;

typedef enum {
    PRIORITY_QUEUE,
    COMPLETED_QUEUE,
} QueueType;

void servicer_init();

void create_priority_queue();

void create_waitlist_queue();

int process_flashcards();

int fsrs_based_queue(Flashcard *card);

int get_queue_capacity(QueueType queue);

int pull_from_queue(QueueType queue, Flashcard *card);

void log_reviewed_card(Flashcard *card, int difficulty);

int edit_flashcard_attribute(Flashcard *card, int choice, char *edit);

int submit_flashcard_data(char *en_word, char *es_word);

ScoreOutcome score_english_translation(Flashcard *card, char *en_guess);