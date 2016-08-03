CC=gcc
INC_DIR = inc
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

directories: $(BIN_DIR) $(OBJ_DIR)
$(BIN_DIR):
	mkdir -p $(BIN_DIR)
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

CFLAGS=-std=c99 -Wall -I$(INC_DIR)

LDFLAGS=-Wall -I. -lm
TARGET=BaseI6_updater.exe
SRC= $(wildcard $(SRC_DIR)/*.c)
OBJ= $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: directories $(BIN_DIR)/$(TARGET)



$(BIN_DIR)/$(TARGET): $(OBJ)
	@$(CC) $(CFLAGS) -o $@ $(LDFLAGS) $(OBJ)
	@echo "Linking:" $< "successfully linked."

$(OBJ) : $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiling:" $< "successfully compiled."



.PHONY: clean cleanall

run: all
	./$(BIN_DIR)/$(TARGET) -n 3 -b 115200 -a -r
	
clean:
	@rm -f $(OBJ)
	@echo "Objects successfully cleaned"

cleanall: clean
	@rm -f $(BIN_DIR)/$(TARGET)
	@echo "Binaries successfully cleaned"
			