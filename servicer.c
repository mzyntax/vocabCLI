#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include "servicer.h"
#include "storagelib.h"
#include "cards.h"
#include "logger.h"
#include "flashcard_system/spaced_repetition.h"
#include "flashcard_queue/queue.h"

static struct {
    Queue *priority;
    Queue *completed;
} active_queues;

void servicer_init () {
    int check = set_log_file("logs/servicer_logs");
    if (check == -1) {
        return;
    }
    create_priority_queue();
    create_waitlist_queue();
}

void create_priority_queue() {
    Queue priority;
    initialize_queue(&priority);
    Queue *p = malloc(sizeof(Queue));
    *p = priority;
    active_queues.priority = p;
    log_info("Created Priority Queue");
}

void create_waitlist_queue() {
    Queue completed;
    initialize_queue(&completed);
    Queue *c = malloc(sizeof(Queue));
    *c = completed;
    active_queues.completed = c;
    log_info("Created Waitlist Queue");
}

int process_flashcards (Flashcard *card) {
    FILE *fp;
    int card_size = sizeof(*card);
    int read_result = read_write_file("build/flashcards.bin", &fp);

    if (read_result != 0) {
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    int amount_of_flashcards = size / sizeof(*card);

    if (amount_of_flashcards < 1) {
        return -1;
    }

    log_info("================  REVIEW SESSION START  ================");
    log_info("Processing %d flashcard(s)", amount_of_flashcards);
    log_info("========================================================");

    int i = 0;

    fseek(fp, i * card_size, SEEK_SET);

    for (;i < amount_of_flashcards; i++) {

        size_t card_info = fread(card, card_size, 1, fp);
        if (card_info != 1) { 
            return -1; 
        }
        int queued = fsrs_based_queue(card);
        if (queued == 0) {
            return 0;
        } else if (queued == 1) {
            return 1;
        }
        else {
            log_debug("Function 'fsrs_based_queue' returned %d", queued);
        }
        
    }
    fclose(fp);
    return 0;
}

static const char *state_label(int state) {
    static const char *names[] = {"LEARNING", "REVIEW", "RELEARNING"};
    return (state >= 0 && state <= 2) ? names[state] : "UNKNOWN";
}

static const char *rating_label(int rating) {
    static const char *names[] = {"NA", "EASY", "GOOD", "HARD", "AGAIN"};
    return (rating >= 0 && rating <= 4) ? names[rating] : "UNKNOWN";
}

int fsrs_based_queue(Flashcard *card) {
    int stability = calculate_stability(card);
    bool retained = check_retention(card, stability);

    time_t unix_now = time(NULL);
    int elapsed_days = (int)(unix_now / 86400 - card->last_review / 86400);
    float R = (stability > 0) ? (float)exp(log(0.9) * elapsed_days / stability) : 0.0f;

    log_info(" ");
    log_info("+-------------------------------------------------------------");
    log_info("|  CARD #%d  |  \"%s\"  ->  \"%s\"",
             card->index, card->spanish_word, card->english_word);
    log_info("|  State:  %-12s  |  Rating:  %s",
             state_label(card->state), rating_label(card->rating));
    log_info("+-------------------------------------------------------------");

    if (stability == -2) {
        log_info("|  Stability:  new card  (no prior rating)");
        log_info("|  Decision:   UNREVIEWED  ->  DUE FOR REVIEW");
    } else {
        log_info("|  Stability:  %d day(s)", stability);
        log_info("|  Elapsed:    %d day(s) since last review", elapsed_days);
        log_info("|  Retention:  R = %.4f  [threshold: 0.9000]  %s",
                 R, retained ? "^^ ABOVE  ->  retained" : "vv BELOW  ->  due");
        log_info("|  Decision:   %s", retained ? "RETAINED" : "DUE FOR REVIEW");
    }

    log_info("+-------------------------------------------------------------");

    int queued;
    if (!retained) {
        queued = enqueue(active_queues.priority, card);
        if (queued == -1) {
            log_error("|  [ERROR] Priority queue full — card #%d dropped", card->index);
            log_info("+-->  [ERROR] Priority queue is full");
            return queued;
        }
        log_info("+-->  PRIORITY QUEUE  [%d card(s) queued]",
                 return_queue_size(active_queues.priority));
        return 0;
    } else {
        queued = enqueue(active_queues.completed, card);
        if (queued == -1) {
            log_error("|  [ERROR] Completed queue full — card #%d dropped", card->index);
            log_info("+-->  [ERROR] Completed queue is full");
            return queued;
        }
        log_info("+-->  COMPLETED QUEUE  [%d card(s) queued]",
                 return_queue_size(active_queues.completed));
        return 1;
    }

    return -1;
}

int get_queue_capacity(QueueType queue) {
    int capacity;
    switch (queue) {
        case PRIORITY_QUEUE:
            capacity = return_queue_size(active_queues.priority);
            break;
        case COMPLETED_QUEUE:
            capacity = return_queue_size(active_queues.completed);
            break;
    }
    return capacity;
}


int pull_from_queue(QueueType queue, Flashcard *card) {
    int dq;
    switch (queue) {
    case PRIORITY_QUEUE:
        dq = dequeue(active_queues.priority, card);
        break;
    case COMPLETED_QUEUE:
        dq = dequeue(active_queues.completed, card);
        break;
    }

    if (dq != 0) {  
        return dq; 
    }

    log_info("Pulled Flashcard | Index %d | English Word %s |", card->index, card->english_word);
    return 0;
    }

void log_reviewed_card(Flashcard *card, int rating) {
    log_review_date(card);

    card->rating = rating;
    int update = update_flashcard(card);
    if (update == -1) {
        log_debug("Flashcard could not be updated");
        return;
    }
    log_info("Reviewed and logged Flashcard | Index: %d | English Word: %s |", card->index, card->english_word);
}

int edit_flashcard_attribute(Flashcard *card, int choice, char *edit) {
    choice--;
    switch (choice) {
    case ENGLISH_WORD:
        strncpy(card->english_word, edit, 31);
        break;
    case SPANISH_WORD:
        strncpy(card->spanish_word, edit, 31);
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
    State state = LEARNING;
    Rating rating = NA;
    Flashcard *pcard = &card;

    card.state = state;
    card.rating = rating;
    card.stability = 0;
    card.last_review = 0;
    
    int en_size = strlen(en_word);
    int es_size = strlen(es_word);

    if (en_size < 20 && es_size < 20) {
        strncpy(card.english_word, en_word, 31);
        strncpy(card.spanish_word, es_word, 31);
    } else {
        return -1;
    }

    int result = create_flashcard(pcard);
    if (result != 0) {
        return -1;
    }
    return 0;
}

ScoreOutcome score_english_translation(Flashcard *card, char *en_guess) {
    char *english_word = card->english_word;

    if (strcmp(english_word, en_guess) == 0) {
        return CORRECT_GUESS;
    } else {
        return INCORRECT_GUESS;
    }

    return INTERNAL_ERROR;
}

void log_review_date(Flashcard *card) {
    time_t today = time(NULL);

    card->last_review = today;
    update_flashcard(card);
}