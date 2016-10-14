CFLAGS=-Wall -g -std=c++11 -O3 -flto -fuse-linker-plugin -fopenmp #-D_GLIBCXX_DEBUG -fsanitize=undefined
LDFLAGS=-lm

# Autogenerated dirs
DIRS=obj bin .deps

# Find all dependencies of the 
SRCS=$(wildcard src/*.cpp)
OBJS=$(SRCS:src/%.cpp=obj/%.o)

# Benchmarks
BENCHMARKS=$(wildcard benchmarks/*.csv)

.PHONY: clean test

test: bin/main
	@for b in benchmarks/*.csv; do\
		echo "\n\033[1m----- Running bechmark $$b\033[0m -----"; \
		./bin/main -b $$b -a naive -a parallel;\
	done;

# Main program with dependencies
bin/main: $(OBJS) | bin
	g++ $(OBJS) $(CFLAGS) $(LDFLAGS) -o $@

# Compiles objects and generates dependencies
obj/%.o: src/%.cpp | obj .deps
	g++ $(CFLAGS) -MM -MP -MT $@ $< > .deps/$*.d
	g++ -c $< $(CPPFLAGS) $(CFLAGS) -o $@

# Remove bulid files
clean:
	rm -r $(DIRS)

# Creates missing folders
$(DIRS):
	mkdir -p $@

# Dependencies are made during compilation - don't try to make them
$(DEP_DIR)/%.d:;

# Include dependencies
-include $(DEP_DIR)/*.d
