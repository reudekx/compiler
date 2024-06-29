# Makefile for compiler project located in lang directory

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -g

# Executable name
TARGET = compiler.out

# Source files (excluding backup.cpp)
SRCS = compiler/compiler.cpp compiler/enums.cpp compiler/lexer.cpp compiler/parser.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Header files
HDRS = compiler/enums.h compiler/lexer.h compiler/parser.h

# Default target
all: $(TARGET)

# Rule to link the object files into the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Rule to compile source files into object files
%.o: %.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target to remove object files and the executable
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
