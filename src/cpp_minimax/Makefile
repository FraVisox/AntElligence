# Compiler and flags
CXX = g++ -Ofast  -O3 -g
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
	$(CXX)  $(CXXFLAGS) -c $< -o $@

out/engine.o : engine.cpp $(ENGINE_H)
	$(CXX) $(CXXFLAGS) -c $< -o $@


# Pattern rule for object files
$(OUT_DIR)/%.o: engine/%.cpp $(ENGINE_H)
	$(CXX) $(CXXFLAGS) -c $< -o $@


# Build final targets
test-% : test/test-%.cpp $(ALL_OJB)  
	$(CXX) $(CXXFLAGS) test/$@.cpp $(ALL_OJB) -o test/$@

clean:
	rm -f $(OUT_DIR)/*.o
 
