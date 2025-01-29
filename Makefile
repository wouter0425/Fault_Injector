# Define the compiler
CXX = g++

# Define the directories
INCDIR = lib/include
SRCDIR = lib/src
OBJDIR = obj
BINDIR = bin
INJECTION_RES_DIR = FI_results
SCHED_RES_DIR = results
DIRS_TO_CLEAN = $(INJECTION_RES_DIR) $(SCHED_RES_DIR)

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

reset:
	@echo "Removing results..."
	for dir in $(DIRS_TO_CLEAN); do \
		echo "Super cleaning $$dir"; \
		rm -rf $$dir/* $$dir/.[!.]* $$dir/..?*; \
	done

# Phony targets
.PHONY: all clean reset