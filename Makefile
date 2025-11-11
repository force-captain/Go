# Compiler
CC = gcc
CFLAGS = -Wall -Wextra -Werror -Iinclude -g

LDFLAGS = -lncurses

SRC_DIR := src
BUILD_DIR := build

SRC := $(shell find $(SRC_DIR) -name '*.c')

OBJ := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC))

TARGET = go

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET)

