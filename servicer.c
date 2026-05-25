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

void servicer_init () {
    set_log_file("servicer_logs");
}

void debug_function(char *funcname) {
    log_debug("Function %s has encountered an internal error", funcname); /*Only for external functions where 
                                                                            error tracing is already accounted for*/
}


int create_timecard(Timecard *time) {
    time_t today = time(NULL);

    struct tm *local = localtime(&now);

    time->year = local->tm_year + 1900;
    time->month = local->tm_month + 1;
    time->day = local->tm_day;
    time->hour = local->tm_hour;
    time->minute = local->tm_minute;
    time->second = local->tm_second;
    return 0;
}

int log_reviewed_card(Flashcard *card, int difficulty) {
    TimeCard time;
    create_timecard(&time);

    card->last_review = time;
    card->difficulty = difficulty;
    update_flashcard(card);
}

int edit_flashcard_attribute(Flashcard *card, int edit, void *ptr) {
    switch (edit) {
    case ENGLISH_WORD:
        card->english_word = (char *) ptr;
        break;
    case SPANISH_WORD:
        card->spanish_word = (char *) ptr;
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
    State new_state = LEARNING;
    Flashcard *pcard = &card;

    card.state = LEARNING;
    card.stability = 0
    card.difficulty = 0
    
    int en_size = strlen(en_word);
    int es_size = strlen(es_word);

    if (en_size < 20 && es_size < 20) {
        card.english_word = en_word;
        card.spanish_word = es_word;
    } else {
        return -1;
    }

    int result = create_flashcard(pcard);
    if (result != 0) {
        return -1;
    }
    return 0;
}

int process_flashcards () {
    FILE *fp;
    Flashcard card;
    int card_size = sizeof(card);
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

    for (int i = 0; i < amt; i++) {

        fseek(fp, i * card_size, SEEK_SET);
        size_t card_info = fread(&card, card_size, 1, fp);
        if (card_info != 1) { return -1; }
        
        if (card.last_review == NULL) {

        }
        calculate_stability(&card);
        bool retained = check_retention(&card);
        if (retained == false) {
            queue_flashcard(&card);
        }
        if (retained == -1) {
            debug_function("check_retention");
    }
    }
    fclose(fp);
    return 0;
}

int pull_from_queue(Flashcard *card) {
    int d = dequeue(card);
    if (d != 0) {
        debug_function("dequeue");
    }

}


ScoreOutcome score_english_translation(Flashcard *card, char *en_guess) {
    char *english_word = card->english_word;

    bool correct_answer;

    card->total_attempts += 1;
    edit_total_attempts(card->total_attempts, card);

    if (strcmp(english_word, en_guess) == 0) {
        card->correct_attempts += 1;
        edit_correct_attempts(card->total_attempts, card);
        correct_answer = true;
    } else {
        correct_answer = false;
    }

    int calc_result = familiarity_calculation(card);
    if (calc_result != 0) {
        return INTERNAL_ERROR;
        log_trace("Familiarity calculation failed");
    }
    if (correct_answer == true) {
        return CORRECT_GUESS;
    } else {
        return INCORRECT_GUESS;
    }

    return INTERNAL_ERROR;
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