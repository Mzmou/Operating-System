# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -Wextra

# Sources
MAIN_SOURCES = main.cpp 
WAREHOUSE_SOURCES = warehouse.cpp 

# Targets
MAIN_TARGET = main
WAREHOUSE_TARGET = warehouse

# Default target
all: $(MAIN_TARGET) $(WAREHOUSE_TARGET)

# Rule for main
$(MAIN_TARGET): $(MAIN_SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule for warehouse
$(WAREHOUSE_TARGET): $(WAREHOUSE_SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Clean target
clean:
	rm -f $(MAIN_TARGET) $(WAREHOUSE_TARGET) *.o
