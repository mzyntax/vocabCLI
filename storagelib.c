#include <stdio.h>
#include <string.h>
#include "cards.h"
#include "logger.h"

void storagelib_init () {
    set_log_file("logs/storagelib_logs");
}

int append_to_file(char *filename, FILE **fp) {
    *fp = fopen(filename, "ab+");

    if (*fp == NULL) {
        return -1;
    }

    rewind(*fp);
    return 0;
}

int read_write_file(char *filename, FILE **fp) {
    *fp = fopen(filename, "rb+");

    if (*fp == NULL) {
        int attempt = append_to_file(filename, fp);
        if (attempt != 0) {
            return -1;
        } else {
            rewind(*fp);
        }
    }
    
    return 0;
}


int update_flashcard (Flashcard *card) {
    FILE *fp;
    int card_size = sizeof(*card);
    Flashcard cpycard;

    cpycard.index = card->index;
    cpycard.last_review = card->last_review;
    cpycard.state = card->state;
    cpycard.rating = card->rating;
    cpycard.stability = card->stability;
    
    strncpy(cpycard.english_word, card->english_word, 31);
    strncpy(cpycard.spanish_word, card->spanish_word, 31);

    int rwf = read_write_file("build/flashcards.bin", &fp);

    if (rwf != 0) {
        log_debug("Function Error, read_write file return -1");
    }

    int result = fseek(fp, card->index * card_size, SEEK_SET);

    if (result != 0) {
        return -1;
    }

    size_t written = fwrite(&cpycard, card_size, 1, fp);

    if (written != 1) {
        return -1;
    }

    fclose(fp);
    return 0;
}


int create_flashcard (Flashcard *card) {
    FILE *fp;

    int read_result = read_write_file("build/flashcards.bin", &fp);
    if (read_result != 0) {
        return -1;
    }

    fseek(fp, 0, SEEK_END); 

    long file_size = ftell(fp);

    fclose(fp);

    card->index = file_size / sizeof(Flashcard);

    int f_result = append_to_file("build/flashcards.bin", &fp);

    if (f_result != 0) {
        return -1;
    }

    size_t written = fwrite(card, sizeof(Flashcard), 1, fp);

    if (written != 1) {
        return -1;
    }

    log_trace("Creating New Flashcard: Index=%d | Front='%s' | Back='%s'",
                card->index, card->english_word, card->spanish_word);

    fclose(fp);
    return 0;
}


int query_flashcard (int index, Flashcard *card) {
    FILE *fp;
    int result = read_write_file("build/flashcards.bin", &fp);
    
    if (result != 0) {
        log_trace("Error occured in read_write_file");
        return -1;
    }

    fseek(fp, index * sizeof(*card), SEEK_SET);
    
    size_t query = fread(card, sizeof(*card), 1, fp);

    if (query == 0) {
        log_trace("Error occured in fread");
        return -1;
    }

    fclose(fp);
    return 0;
}
