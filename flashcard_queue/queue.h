#pragma once

#include <stdbool.h>
#include "../cards.h"
#include "../logger.h"

typedef struct {
    Flashcard *items[100]; // Pointers allow flashcard values to change whilst in Queue
    int capacity;
    int front;
    int rear;
    bool initialized;
} Queue;

void queue_init();

void initialize_queue(Queue *q);

int check_if_empty(Queue *q);

int check_if_full(Queue *q);

int return_queue_size(Queue *q);

int enqueue(Queue *q, Flashcard *card);

int dequeue(Queue *q, Flashcard *card);