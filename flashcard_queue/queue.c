#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

void queue_init () {
    set_log_file("queue_logs");
}

static struct {
    Flascard items[100];
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


int check_if_empty() {
    if (queue.capacity == 100) {
        log_info("Queue is currently empty, could not dequeue");
        return -1;
    } else {
        return 0;
    }
    return -1;
}

int check_if_full(int size) {
    if (queue.capacity < size) {
        log_info("Queue is currently full, could not enqueue");
        return -1;
    } else {
        return 0;
    }
    return -1;
}

int enqueue(Flashcard *card, int size) {
    int c = check_if_full(sizeof(&card));

    if (c == -1) {
        return c;
    }

    int rear = queue.rear++;
    queue.capacity -= size;
    queue.items[rear] = value;
    return 0
}

int dequeue(Flashcard *card) {
    int e = check_if_empty()

    if (e == -1) {
        return e;
    }

    int front = queue.front++;
    &card = queue.items[front];
    return 0;
}

int queue_flashcard(Flashcard *card) {
    initialize_queue();
    int queued = enqueue(card, sizeof(*card));
    if (queued != 0) {
        return queued;
    }
}