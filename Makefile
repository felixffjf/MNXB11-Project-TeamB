# Set the compiler to g++
CXX := g++

# Compiler warnings, optimizations, and C++ standard
CXXWARNINGS := -Wall -Wextra -Werror
CXXOPT := -O3
CXXSTD := -std=c++17

ROOTINC := $(shell root-config --incdir)
ROOTLIBS := $(shell root-config --glibs)


# Include directories
INCLUDES := -I include -I external/include -I$(ROOTINC) # Include CLI11 headers

# Compiler flags
CXXFLAGS := $(CXXWARNINGS) $(CXXSTD) $(CXXOPT) $(INCLUDES)

# Linker flags (empty for now, but can be used for future libraries)
LDFLAGS := $(ROOTLIBS)

# Phony target so 'make clean' works without considering it a real file
.PHONY: all clean

# Target to build the main executable
all: main

# Link main.cxx with object files (add other object files as needed)
main: main.cxx
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to build object files from .cxx files in src/
src/%.o: src/%.cxx
	$(CXX) $(CXXFLAGS) $^ -c -o $@

# Clean up generated object files and executables
clean:
	rm -v src/*.o main

