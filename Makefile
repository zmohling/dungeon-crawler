CC=gcc
CXX=g++
ECHO=echo
LINT=cpplint
RM=rm -rvf
MKDIR=mkdir -p

CFLAGS=-I src -Wall -ggdb3 -funroll-loops -fstack-protector-strong
CXXFLAGS=-I src -Wall -ggdb3 -funroll-loops -fstack-protector-strong
LDFLAGS=-lm -lncurses 

BIN=dungeon_crawler
BUILD_DIR=build
SRC_DIR=src

C_SRC=$(wildcard **/*.c)
CXX_SRC=$(wildcard **/*.cc)
OBJ=$(C_SRC:%.c=$(BUILD_DIR)/%.o) \
	$(CXX_SRC:%.cc=$(BUILD_DIR)/%.o)
DEP=$(OBJ:%.o=%.d)

$(BIN): $(OBJ)
	@$(ECHO) Linking compiled files... 
	@$(CXX) $(LDFLAGS) $^ -o $(@F)
	@$(ECHO) Done. 

-include $(DEP)

$(BUILD_DIR)/%.o: %.c
	@$(ECHO) Compiling $<...
	@$(MKDIR) $(@D)
	@$(CC) $(CFLAGS) -MMD -c $< -o $@

$(BUILD_DIR)/%.o: %.cc
	@$(ECHO) Compiling $<...
	@$(MKDIR) $(@D)
	@$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

.PHONY: clean lint
clean:
	@$(ECHO) Removing all generated files and executables...
	@$(RM) $(BUILD_DIR) $(BIN) $(SRC_DIR)/lex.yy.c $(SRC_DIR)/def_compiler.tab.* \
		core vgcore.* valgrind*
	@$(ECHO) Done.

lint:
	@$(ECHO) Linting source files per Google\'s CXX Styleguide...
	@$(LINT) $(SRC_DIR)/*.c $(SRC_DIR)/*.cc $(SRC_DIR)/*.h
	@$(ECHO) Done.
