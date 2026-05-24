#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

static struct {
    int items[100];
    int capacity;
    int front;
    int rear;
    bool initialized;
} queue;


void initialize_queue() {
    if (queue.initialized == true) {
        return;
    }
    queue.capacity = 100;
    queue.front = -1;
    queue.rear = -1;
    queue.initialized = true;
}

int enqueue(Flashcard *card, int size) {
    cf = check_if_full(sizeof(&card));

    if (c == 1) {
        return;
    }

    int rear = queue.rear++;
    queue.capacity -= size;
    queue.items[rear] = value;
}

int dequeue(Flashcard *card) {
    int e = check_if_empty()

    if (e == -1) {
        return;
    }

    int front = queue.front++;
    &card = queue.items[front];
}

int check_if_empty() {
    if (queue.capacity == 100) {
        return -1
    } else {
        return 0
    }
}

int check_if_full(int size) {
    if (queue.capacity < size) {
        return -1;
    } else {
        return 0;
    }
}
