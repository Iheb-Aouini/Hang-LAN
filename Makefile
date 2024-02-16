CFLAGS = -Wall -Wextra -std=c99
OBJ_DIR = console_hangman/obj
CFLAGS := -Wall -g -w
GUI_FLAGS := `pkg-config --cflags --libs gtk+-3.0`
SOURCE_DIR := console_hangman
HEADER_FILE := $(SOURCE_DIR)/headers.h

all: hangman program_entry

hangman: $(OBJ_DIR)/main.o $(OBJ_DIR)/tree.o $(OBJ_DIR)/game.o $(OBJ_DIR)/afficherTousLesMots.o $(OBJ_DIR)/afficherArbre.o $(OBJ_DIR)/server.o $(OBJ_DIR)/client.o
	gcc $(CFLAGS) $^ -o $(SOURCE_DIR)/$@

program_entry: program_entry.c
	gcc program_entry.c -o program_entry $(GUI_FLAGS)

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.c $(HEADER_FILE) | $(OBJ_DIR)
	gcc $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(SOURCE_DIR)/hangman program_entry
