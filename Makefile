NAME = LineEngine

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17
INCLUDES = `sdl2-config --cflags`
LDFLAGS  = `sdl2-config --libs`  -lGLEW -lGL
SRC_DIR = source
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin
HDRS = hdrs

SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(BIN_DIR)/$(NAME)

$(BIN_DIR)/$(NAME): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@ $(LDFLAGS)
	@echo "✅ Build complete: $@"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HDRS)/*.hpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(BUILD_DIR)

re: fclean all

.PHONY: all clean fclean re
