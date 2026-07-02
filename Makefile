APUE_DIR := apue.3e
APUE_INC := $(APUE_DIR)/include
APUE_HEADER := $(APUE_INC)/apue.h

BUILD_DIR := build
BIN_DIR := bin

APUE_LIB_DIR := $(APUE_DIR)/lib
APUE_BUILD_DIR := $(BUILD_DIR)/apue
APUE_LIB := $(APUE_BUILD_DIR)/libapue.a
APUE_EXCLUDE_SRCS := \
	$(APUE_LIB_DIR)/Orecvfd.c \
	$(APUE_LIB_DIR)/nspipe.c \
	$(APUE_LIB_DIR)/semaph.c \
	$(APUE_LIB_DIR)/sleep.c \
	$(APUE_LIB_DIR)/strerror.c
APUE_LIB_SRCS := $(filter-out $(APUE_EXCLUDE_SRCS),$(wildcard $(APUE_LIB_DIR)/*.c))
APUE_OBJS := $(patsubst $(APUE_LIB_DIR)/%.c,$(APUE_BUILD_DIR)/%.o,$(APUE_LIB_SRCS))

CC := gcc
CXX := g++

CPPFLAGS := -I$(APUE_INC)
CFLAGS := -Wall -Wextra -g
CXXFLAGS := -Wall -Wextra -g -std=c++17

CHAPTER_DIRS := $(sort $(wildcard chapter*))
C_SRCS := $(foreach dir,$(CHAPTER_DIRS),$(wildcard $(dir)/*.c))
CXX_SRCS := $(foreach dir,$(CHAPTER_DIRS),$(wildcard $(dir)/*.cc))

C_TARGETS := $(patsubst %.c,$(BIN_DIR)/%,$(C_SRCS))
CXX_TARGETS := $(patsubst %.cc,$(BIN_DIR)/%,$(CXX_SRCS))
TARGETS := $(C_TARGETS) $(CXX_TARGETS)

COMPILE_COMMANDS := compile_commands.json

.PHONY: all clean list clangd format

all: $(TARGETS)

list:
	@printf '%s\n' $(TARGETS)

clangd: $(COMPILE_COMMANDS)

format:
	clang-format -i $(C_SRCS) $(CXX_SRCS)

$(APUE_BUILD_DIR)/%.o: $(APUE_LIB_DIR)/%.c $(APUE_HEADER)
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(APUE_LIB): $(APUE_OBJS)
	@mkdir -p $(dir $@)
	$(AR) rcs $@ $^

$(BIN_DIR)/%: %.c $(APUE_LIB) $(APUE_HEADER)
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(APUE_LIB) -o $@

$(BIN_DIR)/%: %.cc $(APUE_LIB) $(APUE_HEADER)
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< $(APUE_LIB) -o $@

$(COMPILE_COMMANDS): Makefile $(APUE_LIB_SRCS) $(C_SRCS) $(CXX_SRCS)
	@printf '[\n' > $@
	@first=1; \
	for src in $(APUE_LIB_SRCS); do \
		if [ $$first -eq 0 ]; then printf ',\n' >> $@; fi; \
		printf '  {"directory":"%s","file":"%s","command":"%s %s %s -c %s"}' \
			"$(CURDIR)" "$$src" "$(CC)" "$(CPPFLAGS)" "$(CFLAGS)" "$$src" >> $@; \
		first=0; \
	done; \
	for src in $(C_SRCS); do \
		if [ $$first -eq 0 ]; then printf ',\n' >> $@; fi; \
		printf '  {"directory":"%s","file":"%s","command":"%s %s %s -c %s"}' \
			"$(CURDIR)" "$$src" "$(CC)" "$(CPPFLAGS)" "$(CFLAGS)" "$$src" >> $@; \
		first=0; \
	done; \
	for src in $(CXX_SRCS); do \
		if [ $$first -eq 0 ]; then printf ',\n' >> $@; fi; \
		printf '  {"directory":"%s","file":"%s","command":"%s %s %s -c %s"}' \
			"$(CURDIR)" "$$src" "$(CXX)" "$(CPPFLAGS)" "$(CXXFLAGS)" "$$src" >> $@; \
		first=0; \
	done; \
	printf '\n]\n' >> $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) $(COMPILE_COMMANDS)
