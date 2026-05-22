#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include "servicer.h"
#include "storagelib.h"
#include "flashcard.h"
#include "logger.h"

// static struct {
//     char *previous_card;
// } hist;

void servicer_init () {
    set_log_file("servicer_logs");
}

int edit_flashcard_attribute(Flashcard *card, int edit, void *ptr) {

    switch (edit) {
    case ENGLISH_WORD:
        card->english_word = (char *) ptr;
        break;
    case SPANISH_WORD:
        card->spanish_word = (char *) ptr;
        break;
    case RECENCY:
        card->recency = *(int *) ptr;
        break;
    case TOTAL_ATTEMPTS:
        card->total_attempts = *(int *) ptr;
        break;
    case CORRECT_ATTEMPTS:
        card->correct_attempts = *(int *) ptr;
        break; 
    }
    int e = update_flashcard(card);
    if (e != 0) {
        return -1;
    }
    return 0;
}

int submit_flashcard_data(char *en_word, char *es_word) {
    Flashcard card;
    Flashcard *pcard = &card;

    card.total_attempts = 0;
    card.correct_attempts = 0;
    card.familiarity = 0;
    card.recency = 0 ;

    int en_size = strlen(en_word);
    int es_size = strlen(es_word);

    printf("en_size %d\n", en_size);
    printf("en_word %s\n", en_word);

    if (en_size < 20 && es_size < 20) {
        card.english_word = en_word;
        card.spanish_word = es_word;
    }

    int result = create_flashcard(pcard);
    if (result != 0) {
        return -1;
    }
    return 0;
}

int shuffle_flashcard (Flashcard *card) {
    FILE *fp;

    int read_result = read_write_file("build/flashcards.bin", &fp);

    if (read_result != 0) {
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    int amt = size / sizeof(*card);

    if (amt < 1) {
        return -1;
    }

    srand(time(NULL));
    int index = rand() % amt;

    fseek(fp, index * sizeof(*card), SEEK_SET);

    size_t card_info = fread(card, sizeof(*card), 1, fp);

    if (card_info != 1) {
        return -1;
    }

    fclose(fp);

    return 0;
}

// int familiarity_calculation(Flashcard *card) {
//     float total_attempts = card->total_attempts;
//     float correct_attempts = card->correct_attempts;
//     double familiarity = correct_attempts / total_attempts;

//     int result = edit_familiarity_score(familiarity, card->index);
//     if (result != 0){
//         return -1;
//     }
//     return 0;
// }


// ScoreOutcome score_english_translation(Flashcard *card, char *en_guess) {
//     char *english_word = card->english_word;

//     bool correct_answer;

//     card->total_attempts += 1;
//     edit_total_attempts(card->total_attempts, card);

//     if (strcmp(english_word, en_guess) == 0) {
//         card->correct_attempts += 1;
//         edit_correct_attempts(card->total_attempts, card);
//         correct_answer = true;
//     } else {
//         correct_answer = false;
//     }

//     int calc_result = familiarity_calculation(card);
//     if (calc_result != 0) {
//         return INTERNAL_ERROR;
//         log_trace("Familiarity calculation failed");
//     }
//     if (correct_answer == true) {
//         return CORRECT_GUESS;
//     } else {
//         return INCORRECT_GUESS;
//     }

//     return INTERNAL_ERROR;
// }