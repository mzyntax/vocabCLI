#pragma once

#include "flashcard.h"

int append_to_file(char *filename, FILE **fp);

int read_write_file(char *filename, FILE **fp);

int update_flashcard (int index, char *english_word, char *spanish_word,
                     float familiarity, float attempts, float correct);

int create_flashcard (Flashcard *card);

int query_flashcard (int index, Flashcard *card);