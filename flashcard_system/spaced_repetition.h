#pragma once

#include <stdbool.h>
#include "../cards.h"

void sr_init();

float calculate_stability(Flashcard *card);

void calculate_learning_state(Flashcard *card);

bool calculate_retention(Flashcard *card);

int calculate_next_review(Flashcard *card);