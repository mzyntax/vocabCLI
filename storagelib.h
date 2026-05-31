#pragma once

#include <stdio.h>
#include "cards.h"

int append_to_file(char *filename, FILE **fp);

int read_write_file(char *filename, FILE **fp);

int update_flashcard (Flashcard *card);

int create_flashcard (Flashcard *card);

int query_flashcard (int index, Flashcard *card);

void storagelib_init ();