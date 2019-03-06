CC=gcc
CXX=g++
ECHO=echo
RM=rm -rvf

CFLAGS=-Wall -ggdb3 -funroll-loops -lm 
CXXFLAGS=-Wall -ggdb3 -funroll-loops -lm 
BUILD_DIR=build
SRC_DIR=src

BIN=dungeon_crawler
SRC=$(wildcard $(SRC_DIR)/*.c)
OBJ=$(SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEP=$(OBJ:%.o=%.d)

$(BIN): $(BUILD_DIR)/$(BIN)

$(BUILD_DIR)/$(BIN): $(OBJ)
	@$(ECHO) Linking compiled files... 
	@$(CC) $(CFLAGS) $^ -o $(@F)
	@$(ECHO) Done. 

-include $(DEP)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@$(ECHO) Compiling $<...
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -MMD -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(ECHO) Compiling $<...
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

.PHONY : clean

clean:
	@$(ECHO) Removing all generated files and executables...
	@$(RM) $(BUILD_DIR) $(BIN) core vgcore.*
	@$(ECHO) Done.
