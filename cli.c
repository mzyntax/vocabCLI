#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "flashcard.h"
#include "servicer.h"
#include "storagelib.h"

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
            break;
        }
        printf("Index: %d | Correct: %.2f | Attempts: %.2f | Familiarity: %.2f | ES: %s | EN: %s\n",
                card.index, card.correct, card.attempts, card.familiarity, card.spanish_word, card.english_word);
        amount ++;
    }
}

void new_flashcard() {
    char en_word[20];
    char es_word[20];

    printf("Enter english word: ");
    scanf("%s", en_word);
    printf("Enter spanish word: ");
    scanf("%s", es_word);

    int result = submit_flashcard_data(en_word, es_word);
    check_failure_response(result, "submit_flashcard_data");
}


void edit_flashcard_options() {
    int index;
    int user_edit_choice;
    char word_edit[20];
    int num_edit;
    Flashcard card;

    printf("Enter Flashcard index");
    scanf("%d", &index);

    while (true) {
        query_flashcard(index, &card);
        printf("Edit options for Flashcard number: %d", index);
        printf("1\nEN-Word: '%s' | 2\nES-Word: '%s' | 3\nFamiliarity: '%.2f' | 4\nAttempts: '%.2f' | 5\nCorrect: '%.2f' (0-5): ",
        card.english_word, card.spanish_word, card.familiarity, card.attempts, card.correct);
        scanf("%d", &user_edit_choice);

        printf("New Insertion: ");

        if (user_edit_choice == 1) {
            scanf("%s", word_edit);
            int en_result = edit_english_word(word_edit, index);
            check_failure_response(en_result, "edit_english_word");

        } else if (user_edit_choice == 2) {
            scanf("%s", word_edit);
            int es_result = edit_spanish_word(word_edit, index);
            check_failure_response(es_result, "edit_spanish_word");

        } else if (user_edit_choice == 3) {
            scanf("%d", &num_edit);
            int fam_result = edit_familiarity_score(num_edit, index);
            check_failure_response(fam_result, "edit_familiarity_score");

        } else if (user_edit_choice == 4) {
            scanf("%d", &num_edit);
            int att_result = edit_total_attempts(num_edit, index);
            check_failure_response(att_result, "edit_total_attempts");

        } else if (user_edit_choice == 5) {
            scanf("%d", &num_edit);
            int cor_result = edit_correct_attempts(num_edit, index);
            check_failure_response(cor_result, "edit_correct_attempts");

        } else if (user_edit_choice == 0) {
            break;
            
        } else {
            printf("Invalid entry!\n");
            break;
        }
    }
}

void quiz_start () {
    Flashcard card;
    char user_response[20];
    while (true) {

        int result = shuffle_flashcard(&card);
        check_failure_response(result, "shuffle_flashcard");

        printf("\n| Beggining Quiz ! | Enter 'Quit' (Q) to exit |\n");
        printf("Card chosen: %s | Familiarity: %.2f | Enter translation: ", card.spanish_word, card.familiarity);

        scanf("%s", user_response);
        to_lower(user_response);

        if (strcmp(user_response, "q") == 0 || strcmp(user_response, "quit") == 0) {
            printf("\nExiting Quiz..\n");
            break;
        }

        ScoreOutcome outcome = score_english_translation(&card, user_response);

        switch (outcome) {
        case CORRECT_GUESS:
            printf("You guessed correctly !\n");
            break;
        case INCORRECT_GUESS:
            printf("You guessed incorrectly..\n");
            break;
        case INTERNAL_ERROR:
            printf("Internal error in function\n");
            break;
        }
    }
}


int main () {
    char user_choice[20];
    while (true) {

        printf("Flashcard Quiz (F) | New Flashcard (N) | Edit Flashcard (E) | All Flashcards (A) |  Quit (Q): ");

        int result = scanf("%s", user_choice);

        if (result == 0){
            printf("\nInvalid input!");
            break;
        }


        to_upper(user_choice);
        
        if (strcmp(user_choice, "F") == 0) {
            quiz_start();
        } else if (strcmp(user_choice, "N") == 0) {
            new_flashcard();
        } else if (strcmp(user_choice, "E") == 0) {
            edit_flashcard_options();
        } else if (strcmp(user_choice, "A") == 0) {
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