CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -Werror -pedantic -I$(INCLUDE_DIR)
LDFLAGS=

CFLAGS_DEBUG=-DBASIC_DEBUG -Og -ggdb3
CFLAGS_RELEASE=-O3
CFLAGS_TEST=-DBASIC_TESTING

LDFLAGS_DEBUG=
LDFLAGS_RELEASE=
LDFLAGS_TEST=-lcmocka

TARGET				:= libbasic.a
BUILD_DIR			:= build
BUILD_DEBUG_DIR		:= $(BUILD_DIR)/debug
BUILD_RELEASE_DIR	:= $(BUILD_DIR)/release
BUILD_TEST_DIR		:= $(BUILD_DIR)/test

INCLUDE_DIR			:= include/basic
SOURCE_DIR			:= src
TEST_DIR			:= $(SOURCE_DIR)/test
SOURCES				:= $(notdir $(wildcard $(SOURCE_DIR)/*.c))
OBJECTS				:= $(SOURCES:%.c=%.o)
DEPS				:= $(OBJECTS:.o=.d)
TEST_SOURCES		:= $(notdir $(wildcard $(TEST_DIR)/*.c))

DEBUG_OBJECTS		:= $(addprefix $(BUILD_DEBUG_DIR)/,$(OBJECTS))
RELEASE_OBJECTS		:= $(addprefix $(BUILD_RELEASE_DIR)/,$(OBJECTS))
TEST_OBJECTS		:= $(addprefix $(BUILD_TEST_DIR)/,$(OBJECTS))

.PHONY: all clean debug release test

all: debug

clean:
	rm -rf $(BUILD_DEBUG_DIR)/*
	rm -rf $(BUILD_RELEASE_DIR)/*
	rm -rf $(BUILD_TEST_DIR)/*

debug: CFLAGS += $(CFLAGS_DEBUG)
debug: LDFLAGS += $(LDFLAGS_DEBUG)
debug: $(BUILD_DEBUG_DIR)/$(TARGET)

release: CFLAGS += $(CFLAGS_RELEASE)
release: LDFLAGS += $(LDFLAGS_RELEASE)
release: $(BUILD_RELEASE_DIR)/$(TARGET)

test: CFLAGS += $(CFLAGS_TEST) $(CFLAGS_DEBUG)
test: LDFLAGS += $(LDFLAGS_TEST) $(LDFLAGS_DEBUG)
test: $(addprefix $(BUILD_TEST_DIR)/,$(TEST_SOURCES:%.c=%))

# GCC generated dependency files
-include $(addprefix $(BUILD_DEBUG_DIR)/,$(DEPS))
-include $(addprefix $(BUILD_RELEASE_DIR)/,$(DEPS))
-include $(addprefix $(BUILD_TEST_DIR)/,$(DEPS))

# Build directory creation ====================================================

DIRS := $(BUILD_DIR) $(BUILD_DEBUG_DIR) $(BUILD_RELEASE_DIR) $(BUILD_TEST_DIR)
$(info $(shell mkdir -p $(DIRS)))

# Debug build =================================================================

$(BUILD_DEBUG_DIR)/$(TARGET): $(DEBUG_OBJECTS)
	ar rcs $@ $^

$(BUILD_DEBUG_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(CC) $(CFLAGS) -MMD -c $< -o $@

# Release build ===============================================================

$(BUILD_RELEASE_DIR)/$(TARGET): $(RELEASE_OBJECTS)
	ar rcs $@ $^

$(BUILD_RELEASE_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(CC) $(CFLAGS) -MMD -c $< -o $@

# Test build ==================================================================

$(BUILD_TEST_DIR)/$(TARGET): $(TEST_OBJECTS)
	ar rcs $@ $^

$(BUILD_TEST_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(CC) $(CFLAGS) -MMD -c $< -o $@

$(BUILD_TEST_DIR)/%: $(TEST_DIR)/%.c $(BUILD_TEST_DIR)/$(TARGET)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
