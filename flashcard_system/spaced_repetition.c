#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include "../cards.h"
#include "../storagelib.h"
#include "../servicer.h"
#include "../flashcard_queue/queue.h"

#define TARGET_RETENTION 0.9

void sr_init() {
    set_log_file("logs/sr_logs");
}

int calculate_stability(Flashcard *card) {
    float S;
    float multiplier;
    float base;

    log_trace("|============================================|");
    log_trace("|          calculate_stability               |");
    log_trace("|============================================|");
    log_trace("| ↓  RECEIVED                                |");
    log_trace("|   Addr: %-14p  Index: %d", (void*)card, card->index);
    log_trace("|   State: %-10s  Rating: %-6s  Stability: %.2f",
        state_label(card->state), rating_label(card->rating), card->stability);
    log_trace("|--------------------------------------------|");

    switch (card->state) {
        case RELEARNING: multiplier = .9; break;
        case LEARNING: multiplier = 1.2; break;
        case REVIEW: multiplier = 1.6; break;
    }

    switch (card->rating) {
        case NA:
            log_trace("| Rating is NA — skipping calculation        |");
            log_trace("|============================================|");
            return 0;
        case EASY:  base = 7.0; break;
        case GOOD:  base = 3.0; break;
        case HARD:  base = 1.0; break;
        case AGAIN: base = 0.5; break;
    }
    S = base * multiplier;
    card->stability = S;

    log_trace("| ↓  CALCULATED                              |");
    log_trace("|   Multiplier: %.2f  (from State: %s)", multiplier, state_label(card->state));
    log_trace("|   S = %.1f x %.2f = %.2f", base, multiplier, S);
    log_trace("|--------------------------------------------|");
    log_trace("| ↓  RETURNING                               |");
    log_trace("|   Addr: %-14p  Index: %d", (void*)card, card->index);
    log_trace("|   Stability: %.2f", card->stability);
    log_trace("|============================================|");

    return 0;
}

void calculate_learning_state(Flashcard *card) {
    log_trace("|============================================|");
    log_trace("|        calculate_learning_state            |");
    log_trace("|============================================|");
    log_trace("| ↓  RECEIVED                                |");
    log_trace("|   Addr: %-14p  Index: %d", (void*)card, card->index);
    log_trace("|   State: %-10s  Correct: %d  Incorrect: %d",
        state_label(card->state), card->correct_tally, card->incorrect_tally);
    log_trace("|--------------------------------------------|");

    int correct_tally = card->correct_tally;
    int incorrect_tally = card->incorrect_tally;

    float effective_correct = correct_tally - (incorrect_tally * incorrect_tally);
    if (effective_correct < 0) effective_correct = 0;
    if (effective_correct > 8) effective_correct = 8;

    float learn_rate = 1.0f + 0.15f * effective_correct;

    if (learn_rate <= 1.0f) {
        card->state = RELEARNING;
    } else if (learn_rate >= 1.0f && learn_rate <= 1.7f) {
        card->state = LEARNING;
    } else {
        card->state = REVIEW;
    }

    log_trace("| ↓  RETURNING                               |");
    log_trace("|   Addr: %-14p  Index: %d", (void*)card, card->index);
    log_trace("|   State: %s", state_label(card->state));
    log_trace("|============================================|");
}

bool calculate_retention(Flashcard *card) {
    log_trace("|============================================|");
    log_trace("|          calculate_retention               |");
    log_trace("|============================================|");
    log_trace("| ↓  RECEIVED                                |");
    log_trace("|   Addr: %-14p  Index: %d", (void*)card, card->index);
    log_trace("|   Stability: %.2f  Retention: %.4f", card->stability, card->retention);
    log_trace("|--------------------------------------------|");

    if (card->rating == NA) {
        return false;
    }

    time_t unix_now = time(NULL);

    int elapsed_days = unix_now / 86400 - card->last_review / 86400;

    float R = exp(log(TARGET_RETENTION) * elapsed_days / card->stability);

    card->retention = R;

    log_trace("| ↓  RETURNING                               |");
    log_trace("|   Addr: %-14p  Index: %d", (void*)card, card->index);
    log_trace("|   Retention: %.4f", card->retention);
    log_trace("|============================================|");

    if (R < TARGET_RETENTION) {
        return false;
    } else {
        return true;
    }
}

void calculate_next_review(Flashcard *card) {
    log_trace("|============================================|");
    log_trace("|          calculate_next_review             |");
    log_trace("|============================================|");
    log_trace("| ↓  RECEIVED                                |");
    log_trace("|   Addr: %-14p  Index: %d", (void*)card, card->index);
    log_trace("|   Stability: %.2f  Last Review: %ld", card->stability, card->last_review);
    log_trace("|--------------------------------------------|");

    time_t unix_now = time(NULL);
    int elapsed_days = unix_now / 86400 - card->last_review / 86400;
    int constant = 0;

    while (true) {
        constant++;
        float ret_in_x = exp(log(TARGET_RETENTION) * constant / card->stability);
        if (ret_in_x <= TARGET_RETENTION) {
            card->next_review = constant - elapsed_days;
            break;
        }
    }

    log_trace("| ↓  RETURNING                               |");
    log_trace("|   Addr: %-14p  Index: %d", (void*)card, card->index);
    log_trace("|   Next Review in: %d days", card->next_review);
    log_trace("|============================================|");
}
