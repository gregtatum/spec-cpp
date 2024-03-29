include makefile-icu.mk

DEBUG := -g -O0
CFLAGS := -Wall -Werror -stdlib=libc++ -std=c++2a -pthread $(DEBUG)
LIB := $(ICULIBS)
INCLUDES := -I includes

SOURCES := $(shell find src -type f -name *.cpp)
CODE_OBJECTS := $(patsubst src/%,build/%,$(SOURCES:.cpp=.o))

#Compile to objects
build/%.o: src/%.cpp
	@mkdir -p $(shell echo $@ | sed -e 's/\/[^\/]*\.o//g')
	clang++ $(CFLAGS) $(INCLUDES) -c -o $@ $<

#Link binary file
main: $(CODE_OBJECTS)
	@echo " Link Code: clang++ $^ -o main $(LIB)"; clang++ $^ -o main $(CFLAGS) $(LIB)

clean:
	@echo " Cleaning...";
	@echo " rm -r ./build"; rm -r ./build
	# @echo " rm ./main ./test"; rm ./main ./test

test:
	@echo "Building then running tests"
	@clear
	@make && ./main

test-debug:
	@echo "Building then running tests"
	@clear
	@make && lldb ./main

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

# -fno-asynchronous-unwind-tables
#   Disable call frame information injected into ASM
#   https://stackoverflow.com/questions/2529185/what-are-cfi-directives-in-gnu-assembler-gas-used-for/16007194#16007194
# -S
#   Compile to assembly
# -O0
#   Unoptimized
make asm/%/O0.asm: asm/%/main.cpp
	clang++ -std=c++2a -fno-asynchronous-unwind-tables -S -o $@ $<
	@echo "\nCompiled code:"
	@echo "==================================================="
	@cat $@

# -O3
#   Optimized build, same level as Gecko.
make asm/%/O3.asm: asm/%/main.cpp
	clang++ -std=c++2a -O3 -fno-asynchronous-unwind-tables -S -o $@ $<
	@echo "\nCompiled code:"
	@echo "==================================================="
	@cat $@
