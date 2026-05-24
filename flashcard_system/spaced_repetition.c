#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include "flashcard.h"

int review_card(Flashcard *card) {
    int rating = card->rating;
    float difficulty = card->difficulty;
    
}

int determine_position(Flashcard *card) {
    float retention_rate = exp(log(0.9) * card->elapsed_days / card->stability);
}