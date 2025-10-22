# Compiler
CC = gcc
CFLAGS = -Wall -Wextra -Werror -Iinclude

SRC := $(shell find src -name '*.c')
OBJ := $(SRC:.c=.o)

TARGET = go

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@


%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f $(OBJ) $(TARGET)

