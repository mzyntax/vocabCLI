
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "cards.h"
#include "servicer.h"
#include "storagelib.h"
#include "logger.h"

static struct {
    Flashcard *cards[5];
    int unfreed;
} heap;

void card_init () {
    set_log_file("logs/cards_log");
}

void free_heap_cards() {
    for (int i = 0; i < heap.unfreed; i++) {
        free(heap.cards[i]);
        heap.cards[i] = NULL;
        log_trace("Free'd card at index => |%d|", i);
    }
    heap.unfreed = 0;
}

Flashcard *create_card_skeleton () {
    Flashcard *card = malloc(sizeof(Flashcard));
    heap.cards[heap.unfreed] = card;

    log_trace("Created flashcard at address => |%p|", (void*)card);
    log_trace("Heap'd flashcard at index => |%d|", heap.unfreed);
    heap.unfreed++;
    return card;
}