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

static const char *state_label(int state) {
    static const char *names[] = {"LEARNING", "REVIEW", "RELEARNING"};
    return (state >= 0 && state <= 2) ? names[state] : "UNKNOWN";
}

static const char *rating_label(int rating) {
    static const char *names[] = {"NA", "EASY", "GOOD", "HARD", "AGAIN"};
    return (rating >= 0 && rating <= 4) ? names[rating] : "UNKNOWN";
}

static Flashcard *new_fcard_pointer() {
    Flashcard *pcard = malloc(sizeof(Flashcard));
    return pcard;
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

int process_flashcards () {
    FILE *fp;
    Flashcard x;
    int card_size = sizeof(x);
    int read_result = read_write_file("build/flashcards.bin", &fp);

    if (read_result != 0) {
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    int amount_of_flashcards = size / card_size;

    if (amount_of_flashcards < 1) {
        return -1;
    }

    log_info("================  REVIEW SESSION START  ================");
    log_info("Processing %d flashcard(s)", amount_of_flashcards);
    log_info("========================================================");

    int i = 0;

    fseek(fp, i * card_size, SEEK_SET);

    for (;i < amount_of_flashcards; i++) {
        Flashcard *card = new_fcard_pointer();
        size_t card_info = fread(card, card_size, 1, fp);
        if (card_info != 1) { 
            return -1; 
        }

        log_info("Queuing card #%d | EN: %s => ES: %s |"
            ,card->index, card->spanish_word, card->english_word);

        int queued = fsrs_based_queue(card);

        printf("Queuing card #%d | Card address (make sure its unique) -> %p\n",card->index, (void *)card);

        if (queued == -1) {
            log_debug("Function 'fsrs_based_queue' returned %d", queued);
            return -1;
        }
    }
    fclose(fp);
    return 0;
}

int fsrs_based_queue(Flashcard *card) {
    int stability = calculate_stability(card);
    bool retained = check_retention(card, stability);

    log_info(" ");
    log_info("+-------------------------------------------------------------");
    log_info("|  CARD #%d  |  \"%s\"  ->  \"%s\"",
             card->index, card->spanish_word, card->english_word);
    log_info("|  State:  %-12s  |  Rating:  %s",
             state_label(card->state), rating_label(card->rating));
    log_info("+-------------------------------------------------------------");
    log_info(" ");

    int queued;
    
    if (retained == false) {
        queued = enqueue(active_queues.priority, card);
        if (queued == -1) {
            log_error("|X--->   Priority queue full — card #%d dropped", card->index);
            return queued;
        }
        log_info("✔ |CARD INDEX %d : PRIORITY QUEUE|  => [%d card(s) queued]",
                card->index, return_queue_size(active_queues.priority));

        return 0;

    } else if (retained == true) {
        queued = enqueue(active_queues.completed, card);
        if (queued == -1) {
            log_error("|X--->   Completed queue full — card #%d dropped", card->index);
            return -1;
        }
        log_info("✔ |CARD INDEX %d : COMPLETED QUEUE| => [%d card(s) queued]",
                card->index, return_queue_size(active_queues.completed));
        
        return 0;
    }

    return -1;
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
        return -1; 
    }
    return 0;
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