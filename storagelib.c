#include <stdio.h>
#include <string.h>
#include "flashcard.h"

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


int new_index(int *index) {
    FILE *fp;

    int rw_result = read_write_file("build/index.bin", &fp);

    if (rw_result != 0) {
        return -1;
    }

    size_t read = fread(index, sizeof(*index), 1, fp);

    if (read != 1) {
        if (feof(fp)) {
            *index += 0;
    }} else if (read == 1) {
        *index += 1;
    }

    size_t written = fwrite(index, sizeof(*index), 1, fp);
    
    if (written != 1) {
        return -1;
    }
    rewind(fp);
    fclose(fp);
    return 0;
}


int update_flashcard (int index, char *english_word, char *spanish_word,
                     float familiarity, float attempts, float correct) {
    FILE *fp;
    Flashcard card;

    card.index = index;
    card.attempts = attempts;
    card.correct = correct;
    card.familiarity = familiarity;
    strncpy(card.english_word, english_word, 20);
    strncpy(card.spanish_word, spanish_word, 20);

    read_write_file("build/flashcards.bin", &fp);

    int result = fseek(fp, index * sizeof(Flashcard), SEEK_SET);

    if (result != 0) {
        return -1;
    }

    size_t written = fwrite(&card, sizeof(Flashcard), 1, fp);

    if (written != 1) {
        return -1;
    }

    fclose(fp);
    return 0;
}


int create_flashcard (Flashcard *card) {
    int index = 0;
    FILE *fp;

    int idx_result = new_index(&index);

    if (idx_result == -1) {
        return -1;
    }

    int f_result = append_to_file("build/flashcards.bin", &fp);

    if (f_result != 0) {
        printf("appened_to_file went wrong");
        return -1;
    }

    size_t written = fwrite(card, sizeof(Flashcard), 1, fp);

    if (written != 1) {
        printf("fwrite went wrong");
        return -1;
    }

    fclose(fp);
    return 0;
}


int query_flashcard (int index, Flashcard *card) {
    FILE *fp;

    int result = read_write_file("build/flashcards.bin", &fp);

    if (result != 0) {
        return -1;
    }
    fseek(fp, index * sizeof(*card), SEEK_SET);
    size_t query = fread(card, sizeof(*card), 1, fp);

    if (query != 1) {
        return -1;
    }
    return 0;
}
