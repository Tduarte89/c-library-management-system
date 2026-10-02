# Compiler and compilation flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2

# Directories
SRC_DIR = src
BIN_DIR = bin

# Targets
TARGET = $(BIN_DIR)/library_system
TARGET_WIN = $(BIN_DIR)/library_system.exe

all: $(TARGET)

$(TARGET): $(SRC_DIR)/main.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(SRC_DIR)/main.c -o $(TARGET)
	@echo "Build successful: $(TARGET)"

windows: $(SRC_DIR)/main.c
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	$(CC) $(CFLAGS) $(SRC_DIR)/main.c -o $(TARGET_WIN)
	@echo "Build successful: $(TARGET_WIN)"

clean:
	@rm -rf $(BIN_DIR) *.dat
	@echo "Cleaned build artifacts and binary data files."

run: all
	./$(TARGET)

.PHONY: all windows clean run
