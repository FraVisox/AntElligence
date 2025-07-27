# Compiler and flags
CXX = g++
SHARED_FLAG =  -fPIC -shared -Ofast  -O3 -std=c++17 
TEST_FLAG = -g -O3  #-std=c++2a -Wall -fsanitize=address -fsanitize=null -fsanitize=leak
OUT_DIR = out

# Sources
SRC_ENGINE = action board direction enums gameboard piece position board_bitset

# Extensions
ENGINE_OBJS = $(addprefix $(OUT_DIR)/, $(addsuffix .o, $(SRC_ENGINE)))
ENGINE_CPPS = $(addprefix engine/, $(addsuffix .cpp, $(SRC_ENGINE)))
ENGINE_H = $(addprefix engine/, $(addsuffix .h, $(SRC_ENGINE)))


ALL_OJB = $(ENGINE_OBJS) out/engine.o out/minimax.o
# Default target
all: $(ENGINE_OBJS)

out/minimax.o : minimax.cpp $(ENGINE_H)
	$(CXX) $(CXXFLAGS) -c $< -o $@

out/engine.o : engine.cpp $(ENGINE_H)
	$(CXX) $(CXXFLAGS) -c $< -o $@


# Pattern rule for object files
$(OUT_DIR)/%.o: engine/%.cpp $(ENGINE_H)
	$(CXX) $(CXXFLAGS) -c $< -o $@


# Build final targets
test-% : test/test-%.cpp $(ALL_OJB)  
	$(CXX) $(TEST_FLAG) test/$@.cpp $(ALL_OJB) -o test/$@

clean:
	rm -f $(OUT_DIR)/*.o
 
