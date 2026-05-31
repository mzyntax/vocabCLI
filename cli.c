#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "cards.h"
#include "servicer.h"
#include "storagelib.h"
#include "logger.h"

void cli_init() {
    set_log_file("logs/cli_logs");
}

void check_failure_response(int response, char *original_func) {
    if (response != 0) {
        printf("\nFunction Error | %s\n", original_func);
    }
}

void to_lower(char *word) {
    for (int i = 0; word[i]; i++) {
        word[i] = tolower(word[i]);
    }
}

void to_upper(char *word) {
    for (int i = 0; word[i]; i++) {
        word[i] = toupper(word[i]);
    }
}

void display_all_flashcards () {
    Flashcard card;
    int amount = 0;
    int result = 0;
    int *presult = &result;

    while (result == 0) {
        *presult = query_flashcard(amount, &card);
        if (*presult == -1) {
            printf("No flashcards found !\n");
            break;
        }
        printf("Index: %d | ES: %s | EN: %s\n",
            card.index, card.spanish_word, card.english_word);
        amount ++;
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

int user_rating () {
    int user_diff;
    printf("Rate flashcard difficulty\n");
    printf("( 1 -> Easy | 2 -> Medium | 3 -> Hard | 4 -> Very Hard )\n=> ");
    scanf("%d", &user_diff);
    if (user_diff > 4) {
        printf("(Input is greater than 4 -> submitting as 4)");
        user_diff = 4;
    } else if (user_diff < 1) {
        printf("(Input is less than 1 -> submitting as 1)");
        user_diff = 1;
    }
    return user_diff;
}

void gather_review_data(Flashcard *card) {
    int user_diff = user_rating();
    log_reviewed_card(card, user_diff);
}

void quiz_user(Flashcard *card) {
        printf("\n| Beginning Review ! | Enter 'Quit' (Q) to exit |\n");
        printf("| Enter translation for: %s | => ", card->spanish_word);

        char user_response[25];
        scanf("%s", user_response);
        to_lower(user_response);

        if (strcmp(user_response, "q") == 0 || strcmp(user_response, "quit") == 0) {
            printf("\nExiting Quiz..\n");
            return;
        }

        ScoreOutcome outcome = score_english_translation(card, user_response);
  
        switch (outcome) {
        case CORRECT_GUESS:
            printf("You guessed correctly !\n");
            gather_review_data(card);
            break;
        case INCORRECT_GUESS:
            printf("You guessed incorrectly..\n");
            gather_review_data(card);
            break;
        case INTERNAL_ERROR:
            printf("Internal error in function\n");
            break;
        }
}

void review_flashcards () {
    Flashcard card;
    QueueType priority = PRIORITY_QUEUE;
    QueueType completed = COMPLETED_QUEUE;

    printf("Checking for flashcards due for review..\n");

    int p = process_flashcards(&card);
    
    if (p == 0) {
        int capacity = get_queue_capacity(priority);
        for (int i = 0; i < capacity; i++) {
            printf("Capacity => %d\n", i);
            printf("I => %d\n", i);
            pull_from_queue(priority, &card);
            quiz_user(&card);
            log_trace("Quized user on Flashcard Index %d", card.index);
            }
    } else if (p == 1) {
        int capacity = get_queue_capacity(completed);
        for (int i = 0; i < capacity; i++) {
            pull_from_queue(completed, &card);
        }
    } else {
        printf("No flashcards found!\n");
        return;
    }
}

void initialize_functions() {
    cli_init();
    queue_init();
    servicer_init();
    storagelib_init();
}


int main () {
    initialize_functions();
    char user_choice[20];

    while (true) {
        printf("Review Flashcards (F) | Create Flashcard (C) | Edit Flashcard (E) | Show My Flashcards (S) | Quit (Q)\n=> ");
        scanf("%s", user_choice); 
    
        to_upper(user_choice);
        
        if (strcmp(user_choice, "F") == 0) {
            review_flashcards();
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