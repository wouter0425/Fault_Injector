# Define the compiler
CXX = g++

# Define the directories
INCDIR = lib/include
SRCDIR = lib/src
OBJDIR = obj
BINDIR = bin

# Define the executable name
EXEC = $(BINDIR)/injector

# Define the source and object files
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Define the include directories
INCLUDES = -I$(INCDIR)

# Define the compiler flags
CXXFLAGS = -Wall -std=c++11 $(INCLUDES)

# Default target
all: $(EXEC)

# Create the binary directory if it doesn't exist
$(BINDIR):
	mkdir -p $(BINDIR)

# Create the object directory if it doesn't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Build the executable
$(EXEC): $(BINDIR) $(OBJDIR) $(OBJS)
	$(CXX) $(OBJS) -o $@

# Compile the source files into object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up the build
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Phony targets
.PHONY: all clean