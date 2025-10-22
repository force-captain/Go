#include <stdio.h>
#include "program.h"
#include <string.h>



int main() {
    printf("What board size do you want?\n>");
    
    
    char buf[4];
    int size;
    int gotsize = 0;

    while (!gotsize) {
        if (fgets(buf, sizeof(buf), stdin)) {
            if (sscanf(buf, "%d", &size) == 1) {
                if (size >= 8 && size <= 19) {
                    gotsize = 1;
                } else {
                    printf("Invalid size! Enter between 8 and 19.\n>");
                }
            } else {
                printf("Please enter an integer.\n>");
            }
        } else {
            printf("Input error. Try again.\n>");
            clearerr(stdin);
        }
    }

    printf("Got size %d", size);

	return 0;
}
