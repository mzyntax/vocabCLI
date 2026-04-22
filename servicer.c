#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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


int edit_familiarity_score(int fam_edit, int index) {
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


int edit_total_attempts(int att_edit, int index) {
    Flashcard card;

    int rquery = query_flashcard(index, &card);
    if (rquery != 0) {
        return -1;
    }

    if (att_edit < card.correct) {
        return -1;
    }

    int rsave = update_flashcard(card.index, card.english_word, card.spanish_word, card.familiarity, att_edit, card.correct);
    if (rsave != 0) {
        return -1;
    }
    return 0;
}

int edit_correct_attempts(int cor_edit, int index) {
    Flashcard card;

    int rquery = query_flashcard(index, &card);
    if (rquery != 0) {
        return -1;
    }

    if (cor_edit > card.attempts) {
        return -1;
    }

    int rsave = update_flashcard(card.index, card.english_word, card.spanish_word, card.familiarity, card.attempts, cor_edit);
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


int shuffle_flashcard (Flashcard *card) {
    FILE *fp;
    int total_cards = 0;

    int read_result = read_write_file("build/flashcards.bin", &fp);
    if (read_result != 1) {
        return -1;
    }

    while (fread(card, sizeof(Flashcard), 1, fp) == 1) {
        total_cards += 1;
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
    return 0;
}


ScoreOutcome score_english_translation(Flashcard *card, char *en_guess) {
    int attempts = card->attempts;
    int correct_attempts = card->correct;
    char *spanish_word = card->spanish_word;
    int index = card->index;

    if (strcmp(spanish_word, en_guess) == 0) {
        attempts += 1;
        correct_attempts += 1;
        int cor_result = edit_correct_attempts(correct_attempts, index);
        int att_result = edit_total_attempts(attempts, index);
        if (cor_result != 0 || att_result != 0) {
            return INTERNAL_ERROR;
        }

        return CORRECT_GUESS;

    } else if (strcmp(spanish_word, en_guess) != 0) {
        attempts += 1;
        int att_res = edit_total_attempts(attempts, index);
        if (att_res != 0) {
            return INTERNAL_ERROR;
        }

        return INCORRECT_GUESS;
    }
}