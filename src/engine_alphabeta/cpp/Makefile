# Compiler and flags
CXX = g++ 
SHARED_FLAG = -Ofast -fPIC -shared -O3
TEST_FLAG = -Ofast -O3# -g -Wall # -g -Wall -fsanitize=address -fsanitize=null -fsanitize=leak
OUT_DIR = out

# Sources
SRC_ENGINE = action board direction enums gameboard piece position
SRC_EGRAPG = embadded_board

# Extensions
ENGINE_OBJS = $(addprefix $(OUT_DIR)/, $(addsuffix .o, $(SRC_ENGINE)))
ENGINE_SOS  = $(addprefix $(OUT_DIR)/, $(addsuffix .so, $(SRC_ENGINE)))
ENGINE_TESTS = $(addprefix $(OUT_DIR)/, $(addsuffix -test.o, $(SRC_ENGINE)))
ENGINE_CPPS = $(addprefix engine/, $(addsuffix .cpp, $(SRC_ENGINE)))
ENGINE_H = $(addprefix engine/, $(addsuffix .h, $(SRC_ENGINE)))



# Special cases
TEST_OBJS = $(ENGINE_TESTS)  \
	$(OUT_DIR)/embadded_board-test.o



# Default target
all: $(ENGINE_OBJS) $(ENGINE_SOS)

# Pattern rule for object files
$(OUT_DIR)/%.o: engine/%.cpp $(ENGINE_H)
	$(CXX) $(CXXFLAGS) -c $< -o $@

#$(OUT_DIR)/%.o: graph_board/%.cpp $(GRAPH_H) 
#	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OUT_DIR)/embadded_board.o: embadded_board.cpp embadded_board.h  $(ENGINE_H)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Pattern rule for shared objects
$(OUT_DIR)/%.so: engine/%.cpp $(ENGINE_H)
	$(CXX) $(SHARED_FLAG) -c $< -o $@

#$(OUT_DIR)/%.so: graph_board/%.cpp $(GRAPH_H) 
#	$(CXX) $(SHARED_FLAG) -c $< -o $@

$(OUT_DIR)/embadded_board.so: embadded_board.cpp embadded_board.h $(ENGINE_H)
	$(CXX) $(SHARED_FLAG) -c $< -o $@

# Pattern rule for test object files
$(OUT_DIR)/%-test.o: engine/%.cpp   $(ENGINE_H)
	$(CXX) $(TEST_FLAG) -c $< -o $@


$(OUT_DIR)/embadded_board-test.o: embadded_board.cpp embadded_board.h  $(ENGINE_H)
	$(CXX) $(TEST_FLAG) -c $< -o $@



# Build final targets
test: $(TEST_OBJS) test.cpp
	$(CXX) $(TEST_FLAG) test.cpp $(TEST_OBJS) -o test

test-1: $(TEST_OBJS) test-1.cpp interface.cpp interface.h
	$(CXX) $(TEST_FLAG) test-1.cpp interface.cpp  $(TEST_OBJS) -o test-1

test-2: $(TEST_OBJS) test-2.cpp interface.cpp interface.h
	$(CXX) $(TEST_FLAG) test-2.cpp interface.cpp  interface.h $(TEST_OBJS) -o test-2



engine.so: $(ENGINE_SOS)  interface.cpp
	$(CXX) -fPIC -shared interface.cpp $(ENGINE_SOS)  -o engine.so


interface.so: interface.h interface.cpp $(ENGINE_SOS)  $(OUT_DIR)/embadded_board.so
	$(CXX) $(SHARED_FLAG) interface.h interface.cpp $(ENGINE_SOS)  $(OUT_DIR)/embadded_board.so -o interface.so

clean:
	rm -f $(OUT_DIR)/*.o $(OUT_DIR)/*.so test engine.so interface.so


