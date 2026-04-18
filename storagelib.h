int append_to_file(char *filename, FILE **fp)

int read_write_file(char *filename, FILE **fp)

int get_all_flashcards(bool print)

int new_index(int *index)

void save_flashcard (int index, char *english_word, char *spanish_word, int familiarity, bool ignore)

void query_flashcard (int index, Flashcard *card)