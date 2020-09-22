CC := clang++
BIN_DIR := .
BUILD_DIR := build
SRC_EXT := cpp
CFLAGS := -Wall -Werror -std=c++2a
LIB :=
INCLUDES :=

SRC_DIR := src
BUILD_DIR := $(BUILD_DIR)
CODE_TARGET := $(BIN_DIR)/main
TEST_TARGET := $(BIN_DIR)/test
SOURCES := $(shell find $(SRC_DIR) -type f -name *.$(SRC_EXT))
CODE_OBJECTS := $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(SOURCES:.$(SRC_EXT)=.o))

#Compile to objects
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.$(SRC_EXT)
	@mkdir -p $(shell echo $@ | sed -e 's/\/[^\/]*\.o//g')
	@echo " Compile Code: $(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<"; $(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

#Link binary file
$(CODE_TARGET): $(CODE_OBJECTS)
	@mkdir -p $(BIN_DIR)
	@echo " Link Code: $(CC) $^ -o $(CODE_TARGET) $(LIB)"; $(CC) $^ -o $(CODE_TARGET) $(CFLAGS) $(LIB)

clean:
	@echo " Cleaning...";
	@echo " $(RM) -r ./$(BUILD_DIR)"; $(RM) -r ./$(BUILD_DIR)
	# @echo " $(RM) ./$(CODE_TARGET) ./$(TEST_TARGET)"; $(RM) ./$(CODE_TARGET) ./$(TEST_TARGET)

test:
	@echo "Building then running tests"
	@clear
	@make && ./main

readme:
	@echo "Building the README.md"
	@echo "Load in the preamble.md"
	@cat preamble.md > README.md
	@echo "Generating the test output"
	@echo '```' >> README.md
	@# Run the test output, strip the colors, then put it in the README
	@./main | sed -r "s/\x1B\[([0-9]{1,2}(;[0-9]{1,2})?)?[mGK]//g" >> README.md
	@echo '```' >> README.md
	@echo "Done!"

.PHONY: clean
.PHONY: test
