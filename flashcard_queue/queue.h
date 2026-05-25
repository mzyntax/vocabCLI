#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>

void initialize_queue();

int enqueue(Flashcard *card, int size);

int dequeue(Flashcard *card);

queue_flashcard(Flashcard *card);