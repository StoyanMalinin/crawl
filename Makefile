# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra
LDFLAGS = -lX11 -lGL -lpthread -lpng -lstdc++fs

# Target executable
TARGET = crawl

# Directories
SRC_DIR = src
OUT_DIR = out

# Source files (add more .cpp files here as needed)
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

# Object files
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OUT_DIR)/%.o,$(SOURCES))

# Default target
all: $(OUT_DIR) $(TARGET)

# Create output directory
$(OUT_DIR):
	mkdir -p $(OUT_DIR)

# Link the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compile source files to object files
$(OUT_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OUT_DIR)
	$(CXX) $(CXXFLAGS) -Wno-all -Wno-extra -c $< -o $@

# Run the program
run: $(TARGET)
	./$(TARGET)

# Clean build artifacts
clean:
	rm -rf $(OUT_DIR) $(TARGET)

# Rebuild everything
rebuild: clean all

# Phony targets
.PHONY: all clean run rebuild
