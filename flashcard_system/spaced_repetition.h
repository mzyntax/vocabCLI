#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include "flashcard.h"

void calculate_stability(Flashcard *card);

void check_retention(Flashcard *card);