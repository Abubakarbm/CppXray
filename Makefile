# ── CppXRay Makefile ──────────────────────────────────────────
CXX	:= g++
CXXFLAGS	:= -std=c++17 -Wall -Wextra -O2 -Iinclude
TARGET	:= cppxray_source
SRCS	:= src/main.cpp src/Renderer.cpp
OBJS	:= $(SRCS:.cpp=.o)


# Default target — builds the program
all:	$(TARGET)


# Link object files into the final executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)


# Compile each .cpp into a .o (depends on all headers)
src/%.o: src/%.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -c $< -o $@


# Build then run in one command
run: $(TARGET)
	./$(TARGET)


# Remove all build artifacts
clean:
	rm -f $(OBJS) $(TARGET)


# Tell make these are not real files
.PHONY:	all run clean
