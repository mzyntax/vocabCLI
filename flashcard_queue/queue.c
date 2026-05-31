#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include "queue.h"


void queue_init() {
    set_log_file("logs/queue_logs");
}

void initialize_queue(Queue *q) {
    q->capacity = 100;
    q->front = 0;
    q->rear = 0;
}


int check_if_empty(Queue *q) { //
    if (q->capacity == 100) {
        log_info("Queue is empty");
        return -1;
    } else {
        return 0;
    }
    return -1;
}

int check_if_full(Queue *q) { //
    if (q->capacity < 1) {
        log_info("Queue is full");
        return -1;
    } else {
        return 0;
    }
    return -1;
}

int return_queue_size(Queue *q) {
    int cards_inserted;
    cards_inserted = 100 - q->capacity;
    log_info("Current cards inserted: %d", cards_inserted);
    return cards_inserted;
}

int enqueue(Queue *q, Flashcard *card) {
    int c = check_if_full(q);

    if (c == -1) {
        return c;
    }

    int rear = q->rear % 100;
    q->rear++;
    q->capacity--;
    q->items[rear] = card;
    return 0;
}

int dequeue(Queue *q, Flashcard *card) {
    int e = check_if_empty(q);

    if (e == -1) {
        return e;
    }

    int front = q->front % 100;
    q->front++;
    *card = *q->items[front];
    return 0;
}