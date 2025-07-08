CC = gcc
CFLAGS = -Wall \
	-I./include

EXEC_NAME = selfQlite

OBJ_DIR = obj
BIN_DIR = bin



SRC = $(shell find src -name '*.c')

OBJ = $(patsubst src/%.c, $(OBJ_DIR)/%.o, $(SRC))


# make sure directories exist
$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

# BUILD OBJECTS

$(OBJ_DIR)/%.o: src/%.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@


# TARGETS =================

# make normal default target
.DEFAULT_GOAL := normal

normal: $(OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$(EXEC_NAME) $(OBJ)

clean:
	rm -rf ./bin
	rm -rf ./obj
