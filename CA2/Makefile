CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -Wextra

# Sources
MAIN_SOURCES = main.cpp 
WAREHOUSE_SOURCES = warehouse.cpp 
PARTS_SOURCES = parts.cpp 

# Targets
MAIN_TARGET = main
WAREHOUSE_TARGET = warehouse
PARTS_TARGET = parts 

# Default target
all: $(MAIN_TARGET) $(WAREHOUSE_TARGET) $(PARTS_TARGET)

# Rule for main
$(MAIN_TARGET): $(MAIN_SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule for warehouse
$(WAREHOUSE_TARGET): $(WAREHOUSE_SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $^


$(PARTS_TARGET): $(PARTS_SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Clean target
clean:
	rm -f $(MAIN_TARGET) $(WAREHOUSE_TARGET) $(PARTS_TARGET_TARGET) *.o