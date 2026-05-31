#pragma once

#include <stdbool.h>
#include "../cards.h"

float calculate_stability(Flashcard *card);

bool check_retention(Flashcard *card, float stability);