#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "cards.h"
#include "servicer.h"
#include "storagelib.h"
#include "logger.h"

void cli_init() {
    set_log_file("logs/cli_logs");
}

static void check_failure_response(int response, char *original_func) {
    if (response == -1) {
        printf("\nFunction Error | %s\n", original_func);
    }
}

static void to_lower(char *word) {
    for (int i = 0; word[i]; i++) {
        word[i] = tolower(word[i]);
    }
}

static void to_upper(char *word) {
    for (int i = 0; word[i]; i++) {
        word[i] = toupper(word[i]);
    }
}

static void display_all_flashcards () {
    Flashcard card;
    int amount = 0;
    int result = 0;
    int *presult = &result;
    bool cards_found = false;

    while (result == 0) {
        *presult = query_flashcard(amount, &card);
        if (*presult == -1) {
            break;
        }
        cards_found = true;
        printf("    ||| Index: %d | ES: %s => EN: %s\n",
            card.index, card.spanish_word, card.english_word);
        amount ++;
    }

    if (cards_found == false) {
        printf("No flashcards found!");
    }
}

void new_flashcard() {
    char en_word[20];
    char es_word[20];

    printf("Enter english word: ");
    scanf("%s", en_word);
    to_lower(en_word);
    printf("Enter spanish word: ");
    scanf("%s", es_word);
    to_lower(es_word);
    

    int result = submit_flashcard_data(en_word, es_word);
    check_failure_response(result, "submit_flashcard_data");
}

void edit_flashcard_options() {
    int index, user_edit_choice;
    char word_edit[30];
    Flashcard card;
    printf("\nEnter Flashcard index: ");
    scanf("%d", &index);

    while (true) {
        int qf = query_flashcard(index, &card);
        if (qf != 0) {
            printf("Invalid index! Check your flashcards\n");
            return;
        }
        printf("Edit options for Flashcard number: %d", card.index);
        printf("\n0) Exit\n1) EN-Word: '%s' | \n2) ES-Word: '%s'|\nChoose Option (0-2): ",
                             card.english_word, card.spanish_word);
        scanf("%d", &user_edit_choice);
        printf("\nNew Insertion => ");

        if (user_edit_choice > 0 && user_edit_choice < 3) {
            scanf("%s", word_edit);
        } else if (user_edit_choice == 0) {
            break;
        } else {
            printf("Invalid entry!\n");
            break;
        }

        int efa = edit_flashcard_attribute(&card, user_edit_choice, word_edit);
        check_failure_response(efa, "edit_flashcard_attribute");
        user_edit_choice = -1;
    }
}

void gather_review_data(Flashcard *card) {
    int user_diff;
    printf("\nRate flashcard difficulty:\n");
    printf("||(1) Easy\n||(2) Medium\n||(3) Hard\n||(4) Very Hard\n=> ");
    scanf("%d", &user_diff);
    if (user_diff > 4) {
        printf("(Input is greater than 4 -> submitting as 4)");
        user_diff = 4;
    } else if (user_diff < 1) {
        printf("(Input is less than 1 -> submitting as 1)");
        user_diff = 1;
    }
    log_reviewed_card(card, user_diff);
}

void quiz_user(Flashcard *card) {
    printf("| Enter translation for %s | => ", card->spanish_word);

    char user_response[25];
    scanf("%s", user_response);
    to_lower(user_response);

    if (strcmp(user_response, "q") == 0 || strcmp(user_response, "quit") == 0) {
        printf("\nExiting Quiz..\n");
        return;
    }
    
    log_trace("|    Before Scoring => Correct: %d | Incorrect: %d", card->correct_tally, card->incorrect_tally);
    ScoreOutcome outcome = score_attempt(card, user_response);
    log_trace("|    After Scoring => Correct: %d | Incorrect: %d", card->correct_tally, card->incorrect_tally);

    switch (outcome) {
    case CORRECT_GUESS:
        printf("        >>You guessed correctly<<\n");
        gather_review_data(card);
        break;
    case INCORRECT_GUESS:
        printf("        >>You guessed incorrectly<<\n");
        gather_review_data(card);
        break;
    case INTERNAL_ERROR:
        printf("Internal error in function\n");
        break;
    }
}

void review_flashcards () {
    QueueType priority = PRIORITY_QUEUE;
    QueueType completed = COMPLETED_QUEUE;
    Flashcard card;
    int capacity;

    printf("Checking for flashcards due for review..\n");

    int total_cards = return_flashcard_amount();
    if (total_cards == -1) {
        printf("No flashcards created yet!\n");
        return;
    }

    for (int i = 0; i < total_cards; i++) {
        Flashcard *c = create_card_skeleton();
        int status = process_flashcard(c, i);
        if (status == -1) {
            return;
        }
    }

    capacity = get_queue_capacity(priority);
    if (capacity != 0) {
        printf("Found flashcards due for review today! Beggining Quiz now...\n");
        for (int i = 0; i < capacity; i++) {
            int status = pull_from_queue(priority, &card);
            if (status == -1) {
                log_debug("Pull from queue failed");
                return;
            }
            quiz_user(&card);
        } return; 
    } else {
        capacity = get_queue_capacity(completed);
        printf("No flashcards due for review today! Cards Completed:\n");
        for (int i = 0; i < capacity; i++) {
            int status = pull_from_queue(completed, &card);
            if (status == -1) {
                log_debug("Pull from queue failed");
                return;
            }
            printf("||CARD #%d|| EN: %s => ES: %s | Day(s) until next review: %d\n",
            card.index, card.english_word, card.spanish_word, card.next_review);
            }
        return;
    }
}

void initialize_functions() {
    cli_init();
    servicer_init();
    storagelib_init();
}


int main () {
    initialize_functions();
    char user_choice[20];

    while (true) {
        printf("\n|| Review Flashcards (F) | Create Flashcard (C) | Edit Flashcard (E) | Show My Flashcards (S) | Quit (Q) |\n|| ==> ");
        scanf("%s", user_choice);
        printf("\n");
    
        to_upper(user_choice);
        if (strcmp(user_choice, "F") == 0) {
            review_flashcards();
            free_heap_cards();
        } else if (strcmp(user_choice, "C") == 0) {
            new_flashcard();
        } else if (strcmp(user_choice, "E") == 0) {
            edit_flashcard_options();
        } else if (strcmp(user_choice, "S") == 0) {
            display_all_flashcards();
        } else if (strcmp(user_choice, "Q") == 0) {
            break;
        } else {
            printf("\nInvalid input!\n");
            continue;
        }
    }

    return 0;
}