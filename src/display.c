#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include "board.h"

int get_term_size(int* r, int* c) {
    struct winsize w;
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) return -1;
    *r = w.ws_row;
    *c = w.ws_col;
    return 0;
}

void draw_board(Board* b) {
    int r, c;
    if (get_term_size(&r, &c) == -1) {
        printf("Error getting terminal size!\n");
    }
    char* board_text = board_get_image(b, c);
    printf("\n\n\n%s", board_text);
    free(board_text);
}
