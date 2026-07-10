ROOT := ..
APUE_INC := $(ROOT)/apue.3e/include
APUE_LIB := $(ROOT)/build/apue/libapue.a

CXX := g++
CPPFLAGS := -I$(APUE_INC)
CXXFLAGS := -Wall -Wextra -g -std=c++17

BIN_DIR := bin
SRCS := $(wildcard *.cc)
TARGETS := $(patsubst %.cc,$(BIN_DIR)/%,$(SRCS))

.PHONY: all clean list format

all: $(TARGETS)

list:
	@printf '%s\n' $(TARGETS)

format:
	clang-format -i $(SRCS)

$(APUE_LIB):
	$(MAKE) -C $(ROOT) build/apue/libapue.a

$(BIN_DIR)/%: %.cc $(APUE_LIB)
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< $(APUE_LIB) -o $@

clean:
	rm -rf $(BIN_DIR)
