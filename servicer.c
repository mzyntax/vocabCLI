#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "servicer.h"
#include "storagelib.h"
#include "flashcard.h"

int edit_english_word(char *en_edit, int index) {
    Flashcard card;

    int rquery = query_flashcard(index, &card);
    if (rquery != 0) {
        return -1;
    }

    int rsave = update_flashcard(card.index, en_edit, card.spanish_word, card.familiarity, card.attempts, card.correct);
    if (rsave != 0) {
        return -1;
    }
    return 0;
}


int edit_spanish_word(char *es_edit, int index) {
    Flashcard card;

    int rquery = query_flashcard(index, &card);
    if (rquery != 0) {
        return -1;
    }

    int rsave = update_flashcard(card.index, card.english_word, es_edit, card.familiarity, card.attempts, card.correct);
    if (rsave != 0) {
        return -1;
    }
    return 0;
}


int edit_familiarity_score(float fam_edit, int index) {
    Flashcard card;

    int rquery = query_flashcard(index, &card);
    if (rquery != 0) {
        return -1;
    }

    int rsave = update_flashcard(card.index, card.english_word, card.spanish_word, fam_edit, card.attempts, card.correct);
    if (rsave != 0) {
        return -1;
    }
    return 0;
}
    


int edit_total_attempts(float increment_total, int index) {
    Flashcard card;

    int rquery = query_flashcard(index, &card);
    if (rquery != 0) {
        return -1;
    }

    if (increment_total < card.correct) {
        return -1;
    }

    int rsave = update_flashcard(card.index, card.english_word, card.spanish_word, card.familiarity, increment_total, card.correct);
    if (rsave != 0) {
        return -1;
    }
    return 0;
}

int edit_correct_attempts(float increment_correct, int index) {
    Flashcard card;

    int rquery = query_flashcard(index, &card);
    if (rquery != 0) {
        return -1;
    }

    if (increment_correct > card.attempts) {
        return -1;
    }

    int rsave = update_flashcard(card.index, card.english_word, card.spanish_word, card.familiarity, card.attempts, increment_correct);
    if (rsave != 0) {
        return -1;
    }
    return 0;
}

int submit_flashcard_data(char *en_word, char *es_word) {
    Flashcard card;
    Flashcard *pcard = &card;

    card.attempts = 0;
    card.correct = 0;
    card.familiarity = 0;
    strncpy(card.english_word, en_word, 20);
    strncpy(card.spanish_word, es_word, 20);

    int result = create_flashcard(pcard);
    if (result != 0) {
        return -1;
    }
    return 0;
}

int familiarity_calculation(Flashcard *card) {
    float attempts = card->attempts;
    float correct_attempts = card->correct;
    double familiarity = correct_attempts / attempts;
    printf("Familiarity %f as a result of correct attempts: %f divided by total attempts: %f\n", familiarity, correct_attempts, attempts);
    int result = edit_familiarity_score(familiarity, card->index);
    if (result != 0){
        return -1;
    }
    return 0;
}

int shuffle_flashcard (Flashcard *card) {
    FILE *fp;
    int total_cards = 0;

    int read_result = read_write_file("build/flashcards.bin", &fp);

    if (read_result != 0) {
        return -1;
    }

    while (fread(card, sizeof(*card), 1, fp) == 1) {
        total_cards += 1;
    }
    
    if (total_cards < 1) {
        return -1;
    }

    int index = rand() % total_cards;

    int seek_result = fseek(fp, index * sizeof(*card), SEEK_SET);

    if (seek_result != 0) {
        return -1;
    }

    const size_t card_info = fread(card, sizeof(*card), 1, fp);

    if (card_info != 1) {
        return -1;
    }

    fclose(fp);

    return 0;
}


ScoreOutcome score_english_translation(Flashcard *card, char *en_guess) {
    float total_attempts = card->attempts;
    float correct_attempts = card->correct;
    int index = card->index;
    char *english_word = card->english_word;

    bool correct_answer;

    if (strcmp(english_word, en_guess) == 0) {
        total_attempts += 1;
        correct_attempts += 1;

        int total_result = edit_total_attempts(total_attempts, index);
        if (total_result != 0) {
            return INTERNAL_ERROR;
        }

        int correct_result = edit_correct_attempts(correct_attempts, index);
        if (correct_result != 0) {
            return INTERNAL_ERROR;
        }

        correct_answer = true;
    } else {
        total_attempts += 1;
        int total_result = edit_total_attempts(total_attempts, index);
        if (total_result != 0) {
            return INTERNAL_ERROR;
        }

        correct_answer = false;
    }

    int calc_result = familiarity_calculation(card);
    if (calc_result != 0) {
        return INTERNAL_ERROR;
    }

    if (correct_answer == true) {
        return CORRECT_GUESS;
    } else {
        return INCORRECT_GUESS;
    }

    return INTERNAL_ERROR;
}