# Usually submodule this and add a rule like:
# COMMON_OUT_DIR=$(BUILD_DIR)
# $(MAKE) -C common

COMMON_OUT_DIR ?= build

CC ?= cc
AR ?= ar

INCLUDES := -Iinclude

SRC := $(wildcard src/common/*.c)
OBJ := $(SRC:src/common/%.c=$(COMMON_OUT_DIR)/%.o)
LIB := $(COMMON_OUT_DIR)/libcommon.a

.PHONY: all
all: $(LIB)

$(LIB): $(OBJ)
	@mkdir -p $(COMMON_OUT_DIR)
	$(AR) rcs $@ $^

$(COMMON_OUT_DIR)/%.o: src/common/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(COMMON_OUT_DIR)

# generate compile commands with bear if u got it!!! 
# very good highly recommended ʕ·ᴥ·ʔ
.PHONY: bear-gen-cc
bear-gen-cc: clean
	bear -- $(MAKE)
