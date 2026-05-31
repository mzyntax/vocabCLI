#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include "../cards.h"
#include "../cards.c"
#include "../storagelib.h"
#include "../servicer.h"
#include "../flashcard_queue/queue.h"

#define TARGET_RETENTION 0.9

int calculate_stability(Flashcard *card) {
    float S;
    float multiplier;
    switch (card->state) {
        case RELEARNING:
            multiplier = .9;
            break;
        case LEARNING:
            multiplier = 1.2;
            break;
        case REVIEW:
            multiplier = 1.6;
            break;
    switch (card->rating) {
        case NA:
            return -2;
        case EASY:
            S = 7.0 * multiplier;
            return S;
        case GOOD:
            S = 3.0 * multiplier;
            return S;
        case HARD:
            S = 1.0 * multiplier;
            return S;
        case AGAIN:
            S = 0.5 * multiplier;
            return S;
        }
    }
    return -1;
}

bool check_retention(Flashcard *card, float stability) {
    time_t unix_now = time(NULL);

    int elapsed_days = unix_now / 86400 - card->last_review / 86400;
    
    float R = exp(log(TARGET_RETENTION) * elapsed_days / stability);

    if (stability == -2) {
        return false;
    } else if (R < TARGET_RETENTION) {
        return false;
    } else {
        return true;
    }
    return -1;
}