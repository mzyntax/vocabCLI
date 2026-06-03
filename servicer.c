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

#define CHECK(x) if (x == -1) {return -1;}

static struct {
    Queue *priority;
    Queue *completed;
} active_queues;

static const char *state_label(int state) {
    static const char *names[] = {"LEARNING", "REVIEW", "RELEARNING"};
    return (state >= 0 && state <= 2) ? names[state] : "UNKNOWN";
}

static const char *rating_label(int rating) {
    static const char *names[] = {"NA", "EASY", "GOOD", "HARD", "AGAIN"};
    return (rating >= 0 && rating <= 4) ? names[rating] : "UNKNOWN";
}

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

int return_flashcard_amount () {
    FILE *fp;
    Flashcard card;
    int c_size = sizeof(card);
    int read_result = read_write_file("build/flashcards.bin", &fp);
    CHECK(read_result);

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);

    int amount_of_flashcards = size / c_size;
    if (amount_of_flashcards < 1) {
        log_trace("Unexpected amount of flashcards");
        return -1;
    }
    return amount_of_flashcards;
}

int process_flashcard(Flashcard *card, int card_index) {
    FILE *fp;
    int c_size = sizeof(*card);

    int read_result = read_write_file("build/flashcards.bin", &fp);
    CHECK(read_result);

    fseek(fp, card_index * c_size, SEEK_SET);
    size_t card_info = fread(card, c_size, 1, fp);
    if (card_info != 1) {
        return -1;
    }

    log_info("|===     ↓  PROCESSING ↓    ===|");

    int queued = fsrs_based_queue(card);

    printf("Queuing card #%d | Address -> %p\n", card->index, (void *)card);
    CHECK(queued);
    fclose(fp);
    return 0;
    }

int fsrs_based_queue(Flashcard *card) {
    int stability = calculate_stability(card);
    bool retained = check_retention(card, stability);
    log_info("| Index #%d | EN: %s | State:  %-12s | Rating:  %s",
    card->index, card->english_word, state_label(card->state), rating_label(card->rating));
    
    if (retained == false) {
        int queued = enqueue(active_queues.priority, card);
        if (queued == -1) {
            log_error("|===     Priority queue full — card #%d dropped", card->index);
            return queued;
        }
        log_info("|CARD INDEX %d : PRIORITY QUEUE|  => [%d card(s) queued]",
                card->index, return_queue_size(active_queues.priority));
        log_info("\n");

        return 0;

    } else if (retained == true) {
        int queued = enqueue(active_queues.completed, card);
        if (queued == -1) {
            log_error("|===     Completed queue full — card #%d dropped", card->index);
            return -1;
        }
        log_info("|CARD INDEX %d : COMPLETED QUEUE| => [%d card(s) queued]",
                card->index, return_queue_size(active_queues.completed));
        log_info("|===        ✔ DONE ✔       ===|");
        log_info("");
        return 0;
    }

    return -1;
}

Flashcard pull_from_queue(QueueType queue) {
    Flashcard card;
    switch (queue) {
    case PRIORITY_QUEUE:
        card = dequeue(active_queues.priority);
        log_info("Dequeued priority card");
        break;
    case COMPLETED_QUEUE:
        card = dequeue(active_queues.completed);
        log_info("Dequeued completed card");
        break;
    }
    return card;
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

void log_reviewed_card(Flashcard *card, int rating) {
    log_review_date(card);

    card->rating = rating;
    int update = update_flashcard(card);
    if (update == -1) {
        log_debug("Flashcard could not be updated");
        return;
    }
    log_info("Reviewed and logged Flashcard | Index: %d | English Word: %s |", card->index, card->english_word);
    printf("Freeing with address %p\n", (void *)&card);
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