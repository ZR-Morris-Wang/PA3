SRC_DIR = ./src
BIN_DIR = ./bin
TARGET = $(BIN_DIR)/cb
CXX = g++
CXXFLAGS = -Wall -O3

SRC_FILES = $(wildcard $(SRC_DIR)/cb_prim_matrix.cpp)
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.cpp=%.o)

$(TARGET): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ_FILES)

%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET)

