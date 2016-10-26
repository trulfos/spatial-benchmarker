ifdef DEBUG
	CFLAGS=-Wall -g -std=c++11 -fopenmp -fsanitize=undefined #-D_GLIBCXX_DEBUG
else
	CFLAGS=-Wall -g -std=c++11 -O3 -flto -fuse-linker-plugin -fopenmp -DNDEBUG
endif


LDFLAGS=-lm

# Autogenerated dirs
DIRS=obj bin .deps test

# Find all dependencies
OBJS_COMMON=$(patsubst src/%.cpp,obj/%.o,$(filter-out %.test.cpp,$(wildcard src/common/*.cpp)))

# Find tests
TESTS=$(patsubst src/%.test.cpp,test/%,$(shell find src/ -name *.test.cpp))
ifdef VIM # Make quickfix in vim work
	TEST_SUFFIX=--ascii 2>&1 | sed 's/^\[-\+\] //'
endif

# Benchmarks
BENCHMARKS=$(patsubst %.results.csv,%,$(wildcard benchmarks/*.results.csv))

.PHONY: clean run test_all

run: bin/bench
	@for b in $(BENCHMARKS); do\
		echo "\n\033[1m----- Running bechmark $$b -----\033[0m"; \
		./bin/bench -b $$b -a naive -a parallel -a rtree;\
	done;

test_all: $(TESTS)
	@for t in $(TESTS); do\
		./$$t $(TEST_SUFFIX);\
	done;

# Compiles objects and generates dependencies
obj/%.o: src/%.cpp | obj .deps
	@mkdir -p $(@D)
	g++ $(CFLAGS) -MM -MP -MT $@ $< > .deps/$(subst /,__,$*).d
	g++ -c $< $(CPPFLAGS) $(CFLAGS) -o $@

test/%: src/%.test.cpp | test .deps
	@mkdir -p $(@D)
	g++ $(CFLAGS) -MM -MP -MT $@ $< > .deps/$(subst /,__,$*).test.d
	g++ $< -g -std=c++11 -lcriterion -o $@

# Remove bulid files
clean:
	rm -rf $(DIRS)

# Creates missing folders
$(DIRS):
	mkdir -p $@

# Dependencies are made during compilation - don't try to make them
.deps/%.d:;

# Include dependencies
-include .deps/*.d

# Main programs with dependencies
pc:=%
.SECONDEXPANSION:
bin/%: $$(patsubst src/$$(pc).cpp,obj/$$(pc).o,$$(filter-out %.test.cpp,$$(wildcard src/$$*/*.cpp))) $(OBJS_COMMON)  | bin
	g++ $^ $(CFLAGS) $(LDFLAGS) -o $@
