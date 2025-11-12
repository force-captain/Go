#include <stddef.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <ncurses.h>
#include "board.h"

// Colours
#define CP_LABELS 1
#define CP_BLACK  2
#define CP_WHITE  3
#define CP_BOARD  4
#define CP_SELECT 5


static void init_ncurses_colours();
static void init_ncurses();
static void end_ncurses();
static void draw_board(Board*, int, int, int);
static void handle_input(Board*, int*, int*, int, Colour*);
static MoveResult attempt_move(Board*, Colour, int, int);

static void init_ncurses() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    if (has_colors()) {
        init_ncurses_colours();
    }
}

static void init_ncurses_colours() {
    start_color();
    use_default_colors();

    init_pair(CP_LABELS, COLOR_WHITE, -1);
    init_pair(CP_BLACK,  COLOR_BLACK, COLOR_YELLOW);
    init_pair(CP_WHITE,  COLOR_WHITE, COLOR_YELLOW);
    init_pair(CP_BOARD,  COLOR_BLACK, COLOR_YELLOW); // Remember to use A_DIM
    init_pair(CP_SELECT, COLOR_BLACK, COLOR_GREEN);
}

static void end_ncurses() {
    endwin();
}

static void draw_board(Board* b, int term_col, int cursor_r, int cursor_c) {
    int board_size = board_get_size(b);
    int rows = board_size * 2;
    int row_len = board_size * 4 - 1;
    int padding = (term_col - row_len) / 2;
    if (padding < 0) padding = 0;

    int start_y = 1;
    int start_x = padding + 3;

    // Column labels
    move(start_y, start_x);
    attron(COLOR_PAIR(CP_LABELS));
    for (int c = 0; c < board_size; c++) {
        printw(" %c  ", 'A' + c);
    }
    attroff(COLOR_PAIR(CP_LABELS));

    // Rows
    for (int r = 1; r < rows; r++) {
        int y = start_y + r;
        move(y, padding);

        if (r % 2 == 0) {
            // Separator
            printw("   ");
            for(int c = 0; c < board_size-1; c++) {
                attron(COLOR_PAIR(CP_BOARD) | A_BOLD | A_DIM);
                printw("---+");
                attroff(COLOR_PAIR(CP_BOARD) | A_BOLD | A_DIM);
            }
            attron(COLOR_PAIR(CP_BOARD) | A_BOLD | A_DIM);
            printw("---");
            attroff(COLOR_PAIR(CP_BOARD) | A_BOLD | A_DIM);
        } else {
            // Meaningful row
            int board_row = r / 2;

            attron(COLOR_PAIR(CP_LABELS));
            printw("%2d ", board_row + 1);
            attroff(COLOR_PAIR(CP_LABELS));

            for (int c = 0; c < board_size; c++) {
                Point p = {.x = c, .y = board_row};
                Colour clr = board_get_colour(b, p);

                int attr;
                if (cursor_r == board_row && cursor_c == c) {
                    attr = COLOR_PAIR(CP_SELECT); 
                    if (clr != NONE) attr |= A_BOLD;
                } else if (clr == BLACK) {
                    attr = COLOR_PAIR(CP_BLACK) | A_BOLD | A_DIM;
                } else if (clr == WHITE) {
                    attr = COLOR_PAIR(CP_WHITE) | A_BOLD | A_DIM;
                } else { 
                    attr = COLOR_PAIR(CP_BOARD) | A_DIM;
                }

                attron(attr);
                if (clr == NONE) {
                    printw("   ");
                } else {
                    printw(" O ");
                }
                attroff(attr);

                if (c < board_size - 1) {
                    attron(COLOR_PAIR(CP_BOARD) | A_DIM);
                    printw("|");
                    attroff(COLOR_PAIR(CP_BOARD) | A_DIM);
                }
            }
        }
    }
    move(start_y + rows + 1, start_x);
    printw("WASD to move, SPACE to play, Q to quit.");
}

static void handle_input(Board* b, int* cursor_r, int* cursor_c, int ch, Colour* turn) {
    int board_size = board_get_size(b);
    switch(ch) {
        case KEY_UP:    if (*cursor_r > 0) (*cursor_r)--; break;
        case KEY_DOWN:  if (*cursor_r < board_size - 1) (*cursor_r)++; break;
        case KEY_LEFT:  if (*cursor_c > 0) (*cursor_c)--; break;
        case KEY_RIGHT: if (*cursor_c < board_size - 1) (*cursor_c)++; break;
        case ' ':
            if (attempt_move(b, *turn, *cursor_r, *cursor_c) == MOVE_OK) {
                *turn = *turn == BLACK ? WHITE : BLACK;
            }
            break;
    }
}

static MoveResult attempt_move(Board* b, Colour colour, int row, int col) {
    Point p = {.x = col, .y = row};
    return board_place_tile(b, colour, p);
}

static int get_term_col() {
    struct winsize w;
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) return -1;
    return w.ws_col;
}

void display_interactive(Board* b) {
    int cursor_r = 0, cursor_c = 0;
    int ch;
    Colour turn = BLACK;
    int term_col = get_term_col();

    init_ncurses();

    clear();
    draw_board(b, term_col, cursor_r, cursor_c);
    refresh();

    while (1) {
        ch = getch();
        if (ch == 'q' || ch == 'Q') break;
        
        handle_input(b, &cursor_r, &cursor_c, ch, &turn);

        clear();
        draw_board(b, term_col, cursor_r, cursor_c);
        refresh();
    }
    end_ncurses();
}

/*
static void set_raw_mode(int);
int get_term_col();
void attempt_move(Board*, Colour, int, int);

void display_interactive(Board* b) {
    set_raw_mode(1);
    int cursor_r = 0, cursor_c = 0;
    int quit = 0;
    Colour turn = BLACK;
    
    while (!quit) {
        system("clear");

        // Draw board
        int term_col = get_term_col();
        char* img = board_get_image(b, term_col);

        printf("\n\n%s\n", img);
        printf("WASD to move, SPACE to place, q to quit.\n");
        printf("Current position: %c%d\n", cursor_c + 'A', cursor_r + 1);
        free(img);

        char c;
        if(read(STDIN_FILENO, &c, 1) != 1) continue;
        int board_size = board_get_size(b);
        switch(c) {
            case 'q': quit = 1; break;
            case 'w': if (cursor_r > 0) cursor_r--; break;
            case 's': if (cursor_r < board_size - 1) cursor_r++; break;
            case 'a': if (cursor_c > 0) cursor_c--; break;
            case 'd': if (cursor_c < board_size - 1) cursor_c++; break;
            case ' ': 
                attempt_move(b, turn, cursor_r, cursor_c);
                turn = turn == BLACK ? WHITE : BLACK;
                break;
        }
    }
}

void attempt_move(Board* b, Colour colour, int row, int col) {
    Point p = {.x = col, .y = row};
    board_place_tile(b, colour, p);
}

static void set_raw_mode(int enable) {
    static struct termios oldt, newt;
    if (enable) {
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
}

int get_term_col() {
    struct winsize w;
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) return -1;
    return w.ws_col;
}*/
