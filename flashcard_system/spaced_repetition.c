#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include "flashcard.h"
#include "storagelib.h"
#include "servicer.h"
#include "queue.h"

#define TARGET_RETENTION 0.9

int calculate_stability(Flashcard *card) {
    float S;
    float multiplier;
    switch (card->state) {
        case RELEARNING:
        multiplier = .9
        case LEARNING:
        multiplier = 1.2
        case REVIEW:
        multiplier = 1.6
    switch (card->difficulty) {
        case EASY:
        S = 7.0 * multiplier;
        case GOOD:
        S = 3.0 * multiplier;
        case HARD:
        S = 1.0 * multiplier;
        case AGAIN:
        S = 0.5 * multiplier;
        }
    }
    card->stability = S;
    update_flashcard(card);
}

int check_retention(Flashcard *card) {
    Timecard time;
    create_timecard(&time);

    elapsed_days = time.today - card->last_review->day;
    float R = exp(log(TARGET_RETENTION) * elapsed_days / card->stability);

    if (R < TARGET_RETENTION) {
        initialize_queue();
        enqueue(card, sizeof(*card));
    };
}