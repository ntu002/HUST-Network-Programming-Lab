#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main() {
    char input[100];
    char letters[100];
    char digits[100];
    int letter_count = 0;
    int digit_count = 0;

    printf("Enter a string: ");
    fgets(input, sizeof(input), stdin);

    for (int i = 0; input[i] != '\0'; i++) {
        if (isalpha(input[i])) {
            letters[letter_count] = input[i];
            letter_count++;
        } else if (isdigit(input[i])) {
            digits[digit_count] = input[i];
            digit_count++;
        } else if (!isspace(input[i])) {
            printf("Error: Invalid character found in the string.\n");
            return 1;
        }
    }

    letters[letter_count] = '\0';
    digits[digit_count] = '\0';

    if (letter_count > 0) {
        printf("Letters: %s\n", letters);
    } else {
        printf("No letters found in the string.\n");
    }

    if (digit_count > 0) {
        printf("Digits: %s\n", digits);
    } else {
        printf("No digits found in the string.\n");
    }

    return 0;
}
