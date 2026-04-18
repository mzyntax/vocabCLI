#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

int append_to_file(char *filename, FILE **fp) {
    *fp = fopen(filename, "ab+");

    if (*fp == NULL) {
        return -1;
    }

    rewind(*fp);
    return 1;
}

int read_write_file(char *filename, FILE **fp) {
    *fp = fopen(filename, "rb+");

    if (*fp == NULL) {
        printf("\nOpening %s Failed, Attemping creation..", filename);
        int attempt = append_to_file(filename, fp);
        if (attempt != 1) {
            printf(".Creation failed\n");
            return -1;
        } else {
            printf(".Creation success\n");
            rewind(*fp);
        }
    }

    return 1;
}

int get_all_flashcards(bool print) {
    FILE *fp;
    Flashcard card;
    int total_cards = 0;

    int result = read_write_file("flashcards.bin", &fp);

    if (result != 1) {
        printf("Get_all_flashcards Error: could read/write to file");
    }

    while (fread(&card, sizeof(Flashcard), 1, fp) == 1) {
        if (print) {
            printf("Index: %d | Familiarity: %d | Ignore: %d | ES: %s | EN: %s\n",
                card.index,
                card.familiarity,
                card.ignore,
                card.spanish_word,
                card.english_word);
            }
        }
        total_cards += 1;
    fclose(fp);
    return total_cards;
}

int new_index(int *index) {
    FILE *fp;

    int result = read_write_file("index.bin", &fp);

    if (result != 1) {
        printf("New_index Error: Couldnt read/write to file");
        return -1;
    }

    size_t read = fread(index, sizeof(*index), 1, fp);

    if (read == 1) {
        printf("\nIncrementing index by 1\n");
        *index += 1;
    } else if (feof(fp)) {
        *index += 0;
        printf("Empty file, Starting index with 0");
    }

    size_t write_result = fwrite(index, sizeof(*index), 1, fp);
    rewind(fp);
    return write_result;
}

void save_flashcard (int index, char *english_word, char *spanish_word, int familiarity, bool ignore) {
    FILE *fp;
    Flashcard card;

    card.index = index;
    card.ignore = 0;
    card.familiarity = familiarity;
    strncpy(card.english_word, english_word, 20);
    strncpy(card.spanish_word, spanish_word, 20);

    read_write_file("flashcards.bin", &fp);

    int result = fseek(fp, index * sizeof(Flashcard), SEEK_SET);

    if (result != 0) {
        printf("Save Flashcard Error: fseek did not succeed");
    }

    size_t written = fwrite(&card, sizeof(Flashcard), 1, fp);

    if (written != 1) {
        printf("save_flashcard: Could not write to file\n");
        return;
    }

    printf("Saved flashcard with index: %d\n", card.index);
    fclose(fp);
}

void query_flashcard (int index, Flashcard *card) {
    FILE *fp;

    int result = read_write_file("flashcards.bin", &fp);

    if (result != 1) {
        printf("Query Flashcard Error: Couldnt read file");
        return;
    }
    fseek(fp, index * sizeof(*card), SEEK_SET);
    size_t query = fread(card, sizeof(*card), 1, fp);

    if (query != 1) {
        printf("Query_flashcard: Couldnt find search match for index number '%d'.", index);
        return;
    }
}