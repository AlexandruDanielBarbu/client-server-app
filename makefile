# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -g  -std=c++20 -Iinclude

# Directories
SRC_DIR = src
OBJ_DIR = build
BIN_DIR = bin

# Source files
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))

# Binaries
TARGETS = server subscriber

# Executable dependencies
SERVER_OBJ = $(OBJ_FILES) $(OBJ_DIR)/server.o
SUBSCRIBER_OBJ = $(OBJ_FILES) $(OBJ_DIR)/subscriber.o

# Default target
all: $(TARGETS)

# Build server
server: $(SERVER_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Build subscriber
subscriber: $(SUBSCRIBER_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compile src/ and top-level cpp files into build/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run targets
run_server: server
	./server 4040

run_subscriber: subscriber
	./subscriber C1 127.0.0.1 4040

# Clean
clean:
	rm -rf $(OBJ_DIR)/*.o $(TARGETS)

# Topic tree testing
topic_tree_test: src/topic_tree.cpp src/node.cpp src/client.cpp
	$(CXX) $(CXXFLAGS) -o topic_tree_test src/topic_tree.cpp src/node.cpp src/client.cpp


.PHONY: all clean run_server run_subscriber
