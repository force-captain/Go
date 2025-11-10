#include <stdio.h>
#include <stdlib.h>
#include "program.h"
#include "board.h"

void take_user_turn(Board* b) {
    int r;
    printf("Enter your move\n>");
    r = (int)board_get_size(b);
    printf("%d", r);
}

void draw_board(Board* b) {
    char* board_text = board_get_image(b);
    printf("\033[H");
    printf("%s", board_text);
    free(board_text);
}

int main() {
    printf("\033[2J");

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

    printf("Got size %d\n", size);
    Board* board = board_init(size);
    draw_board(board);

	return 0;
}
