CC=gcc
CXX=g++
ECHO=echo
RM=rm -rvf
MKDIR=mkdir -p

CFLAGS=-Wall -ggdb3 -funroll-loops -fstack-protector-strong
CXXFLAGS=-Wall -ggdb3 -funroll-loops 
LDFLAGS=-lm -lncurses

BIN=dungeon_crawler
BUILD_DIR=build
SRC_DIR=src

C_SRC=$(wildcard $(SRC_DIR)/*.c)
CPP_SRC=$(wildcard $(SRC_DIR)/*.cpp)
OBJ=$(C_SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o) \
	$(CPP_SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEP=$(OBJ:%.o=%.d)

$(BIN): $(OBJ)
	@$(ECHO) Linking compiled files... 
	@$(CC) $(LDFLAGS) $^ -o $(@F)
	@$(ECHO) Done. 

-include $(DEP)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@$(ECHO) Compiling $<...
	@$(MKDIR) $(@D)
	@$(CC) $(CFLAGS) -MMD -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(ECHO) Compiling $<...
	@$(MKDIR) $(@D)
	@$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

.PHONY: clean

clean:
	@$(ECHO) Removing all generated files and executables...
	@$(RM) $(BUILD_DIR) $(BIN) core vgcore.* valgrind*
	@$(ECHO) Done.
