APUE_DIR := apue.3e
APUE_INC := $(APUE_DIR)/include
APUE_ERROR := $(APUE_DIR)/lib/error.c

CC := gcc
CXX := g++
CPPFLAGS := -I$(APUE_INC)
CFLAGS := -Wall -Wextra -g
CXXFLAGS := -Wall -Wextra -g -std=c++17

CHAPTER_DIRS := $(sort $(wildcard chapter*))
C_SRCS := $(foreach dir,$(CHAPTER_DIRS),$(wildcard $(dir)/*.c))
CXX_SRCS := $(foreach dir,$(CHAPTER_DIRS),$(wildcard $(dir)/*.cc))
TARGETS := $(patsubst %.c,%,$(C_SRCS)) $(patsubst %.cc,%,$(CXX_SRCS))
COMPILE_COMMANDS := compile_commands.json

.PHONY: all clean list clangd

all: $(TARGETS)

list:
	@printf '%s\n' $(TARGETS)

clangd: $(COMPILE_COMMANDS)

$(COMPILE_COMMANDS):
	@printf '[\n' > $@
	@first=1; \
	for src in $(C_SRCS); do \
		if [ $$first -eq 0 ]; then printf ',\n' >> $@; fi; \
		printf '  {"directory":"%s","file":"%s","command":"%s %s %s %s -o %s"}' \
			"$(CURDIR)" "$$src" "$(CC)" "$(CPPFLAGS)" "$(CFLAGS)" "$$src $(APUE_ERROR)" "$${src%.c}" >> $@; \
		first=0; \
	done; \
	for src in $(CXX_SRCS); do \
		if [ $$first -eq 0 ]; then printf ',\n' >> $@; fi; \
		printf '  {"directory":"%s","file":"%s","command":"%s %s %s %s -o %s"}' \
			"$(CURDIR)" "$$src" "$(CXX)" "$(CPPFLAGS)" "$(CXXFLAGS)" "$$src $(APUE_ERROR)" "$${src%.cc}" >> $@; \
		first=0; \
	done; \
	printf '\n]\n' >> $@

%: %.c $(APUE_ERROR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(APUE_ERROR) -o ./bin/$@

%: %.cc $(APUE_ERROR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< $(APUE_ERROR) -o ./bin/$@

clean:
	rm -f $(TARGETS) $(COMPILE_COMMANDS)
