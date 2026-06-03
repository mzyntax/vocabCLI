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
    q->items[rear] = *card;
    return 0;
}

Flashcard dequeue(Queue *q) {
    int e = check_if_empty(q);
    Flashcard card;

    if (e == -1) {
        card.index = -1;
        log_info("Queue was empty, set sentinal");
    }

    int front = q->front % 100;
    q->front++;
    q->capacity++; // capacity needs to increase if an item goes away
    card = q->items[front];
    return card;
}

void print_queue(Queue *q) {
    int empty = check_if_empty(q);
    if (empty == -1){
        log_info("Could not print => empty queue");
    }
    printf("\nCurrent Queue: ");

    for (int i = q->front; i < q->rear; i++) {
        Flashcard card = q->items[i % 100];
        printf("%s", card.english_word);
    }

    printf("\n");

}