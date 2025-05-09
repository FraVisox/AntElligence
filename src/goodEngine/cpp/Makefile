# Compiler and flags
CXX = g++ 
SHARED_FLAG = -Ofast -fPIC -shared -O3
TEST_FLAG = -g -Wall #-Ofast -O3 #-g -Wall # -g -Wall -fsanitize=address -fsanitize=null -fsanitize=leak
OUT_DIR = out

# Sources
SRC_ENGINE = action board direction enums gameboard piece position
SRC_GRAPH = build_from_graph graph_board update_board
SRC_EGRAPG = embadded_board

# Extensions
ENGINE_OBJS = $(addprefix $(OUT_DIR)/, $(addsuffix .o, $(SRC_ENGINE)))
ENGINE_SOS  = $(addprefix $(OUT_DIR)/, $(addsuffix .so, $(SRC_ENGINE)))
ENGINE_TESTS = $(addprefix $(OUT_DIR)/, $(addsuffix -test.o, $(SRC_ENGINE)))
ENGINE_CPPS = $(addprefix engine/, $(addsuffix .cpp, $(SRC_ENGINE)))
ENGINE_H = $(addprefix engine/, $(addsuffix .h, $(SRC_ENGINE)))

GRAPH_OBJS = $(addprefix $(OUT_DIR)/, $(addsuffix .o, $(SRC_GRAPH)))
GRAPH_SOS  = $(addprefix $(OUT_DIR)/, $(addsuffix .so, $(SRC_GRAPH)))
GRAPH_TESTS = $(addprefix $(OUT_DIR)/, $(addsuffix -test.o, $(SRC_GRAPH)))
GRAPH_H = $(addprefix graph_board/, $(addsuffix .o, $(SRC_GRAPH)))



# Special cases
TEST_OBJS = $(ENGINE_TESTS) $(GRAPH_TESTS) \
	$(OUT_DIR)/embadded_board-test.o



# Default target
all: $(ENGINE_OBJS) $(GRAPH_OBJS) $(ENGINE_SOS) $(GRAPH_SOS)

# Pattern rule for object files
$(OUT_DIR)/%.o: engine/%.cpp $(ENGINE_H)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OUT_DIR)/%.o: graph_board/%.cpp $(GRAPH_H) 
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OUT_DIR)/embadded_board.o: embadded_board.cpp embadded_board.h $(GRAPH_H) $(ENGINE_H)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Pattern rule for shared objects
$(OUT_DIR)/%.so: engine/%.cpp $(ENGINE_H)
	$(CXX) $(SHARED_FLAG) -c $< -o $@

$(OUT_DIR)/%.so: graph_board/%.cpp $(GRAPH_H) 
	$(CXX) $(SHARED_FLAG) -c $< -o $@

$(OUT_DIR)/embadded_board.so: embadded_board.cpp embadded_board.h $(GRAPH_H) $(ENGINE_H)
	$(CXX) $(SHARED_FLAG) -c $< -o $@

# Pattern rule for test object files
$(OUT_DIR)/%-test.o: engine/%.cpp   $(ENGINE_H)
	$(CXX) $(TEST_FLAG) -c $< -o $@

$(OUT_DIR)/%-test.o: graph_board/%.cpp $(GRAPH_H) 
	$(CXX) $(TEST_FLAG) -c $< -o $@

$(OUT_DIR)/embadded_board-test.o: embadded_board.cpp embadded_board.h $(GRAPH_H) $(ENGINE_H)
	$(CXX) $(TEST_FLAG) -c $< -o $@



# Build final targets
test: $(TEST_OBJS) test.cpp
	$(CXX) $(TEST_FLAG) test.cpp $(TEST_OBJS) -o test

test-1: $(TEST_OBJS) test-1.cpp interface.cpp interface.h
	$(CXX) $(TEST_FLAG) test-1.cpp interface.cpp  $(TEST_OBJS) -o test-1



engine.so: $(ENGINE_SOS) $(GRAPH_SOS) interface.cpp
	$(CXX) -fPIC -shared interface.cpp $(ENGINE_SOS) $(GRAPH_SOS) -o engine.so

interface.so: interface.cpp engine/*.cpp engine/*.h graph_board/* embadded_board.*
	$(CXX) -fPIC -shared -O3 interface.cpp engine/*.cpp graph_board/* embadded_board.* -o interface.so

slow-interface.so: interface.cpp $(ENGINE_SOS) $(GRAPH_SOS) $(OUT_DIR)/embadded_board.so
	$(CXX) $(SHARED_FLAG) interface.cpp $(ENGINE_SOS) $(GRAPH_SOS) $(OUT_DIR)/embadded_board.so -o interface.so

clean:
	rm -f $(OUT_DIR)/*.o $(OUT_DIR)/*.so test engine.so interface.so


test-fast:  test.cpp  embadded_board.cpp embadded_board.h $(GRAPH_H) $(ENGINE_H)
	$(CXX) -O3 test.cpp engine/*.cpp graph_board/*.cpp graph_board/*.h embadded_board.* -o test-fast


#TEST_FLAG :=-O3
#BUILD_FLAG := -O3 
#
#out/action.o:  engine/action.cpp engine/action.h engine/piece.h engine/position.h engine/direction.h
#	g++  engine/action.cpp -c -o out/action.o
#out/board.o: engine/board.cpp engine/board.h engine/action.h engine/gameboard.h
#	g++  engine/board.cpp -c -o out/board.o
#out/direction.o:  engine/direction.cpp engine/direction.h
#	g++  engine/direction.cpp -c -o out/direction.o
#out/enums.o: engine/enums.cpp engine/enums.h
#	g++  engine/enums.cpp -c -o out/enums.o
#out/gameboard.o: engine/gameboard.cpp engine/gameboard.h engine/piece.h engine/position.h engine/enums.h engine/direction.h
#	g++  engine/gameboard.cpp -c -o out/gameboard.o
#out/piece.o: engine/piece.cpp engine/piece.h engine/enums.h
#	g++  engine/piece.cpp -c -o out/piece.o
#out/position.o: engine/position.cpp engine/position.h engine/direction.h engine/enums.h
#	g++ engine/position.cpp -c -o out/position.o
#
# 
#
# out/board_exp.so: engine/*.cpp engine/*.h
#	g++ $BUILD_FLAG  engine/action.cpp engine/board.cpp engine/direction.cpp 
#
#
#out/action.so:   engine/action.cpp engine/action.h engine/piece.h engine/position.h engine/direction.h
#	g++ -fPIC engine/action.cpp -c -o out/action.so
#out/board.so: engine/board.cpp engine/board.h engine/action.h engine/gameboard.h
#	g++ -fPIC engine/board.cpp -c -o out/board.so
#out/direction.so:  engine/direction.cpp engine/direction.h
#	g++ -fPIC engine/direction.cpp -c -o out/direction.so
#out/enums.so: engine/enums.cpp engine/enums.h
#	g++ -fPIC engine/enums.cpp -c -o out/enums.so
#out/gameboard.so: engine/gameboard.cpp engine/gameboard.h engine/piece.h engine/position.h engine/enums.h engine/direction.h
#	g++ -fPIC engine/gameboard.cpp -c -o out/gameboard.so
#out/piece.so: engine/piece.cpp engine/piece.h engine/enums.h
#	g++ -fPIC engine/piece.cpp -c -o out/piece.so
#out/position.so: engine/position.cpp engine/position.h engine/direction.h engine/enums.h
#	g++ -fPIC engine/position.cpp -c -o out/position.so
#
#
# 
#
# 
#out/action-test.o:  engine/action.cpp engine/action.h engine/piece.h engine/position.h engine/direction.h
#	g++ $(TEST_FLAG) engine/action.cpp -c -o out/action-test.o
#out/board-test.o: engine/board.cpp engine/board.h engine/action.h engine/gameboard.h
#	g++ $(TEST_FLAG) engine/board.cpp -c -o out/board-test.o
#out/direction-test.o:  engine/direction.cpp engine/direction.h
#	g++ $(TEST_FLAG) engine/direction.cpp -c -o out/direction-test.o
#out/enums-test.o: engine/enums.cpp engine/enums.h
#	g++ $(TEST_FLAG) engine/enums.cpp -c -o out/enums-test.o
#out/gameboard-test.o: engine/gameboard.cpp engine/gameboard.h engine/piece.h engine/position.h engine/enums.h engine/direction.h
#	g++ $(TEST_FLAG) engine/gameboard.cpp -c -o out/gameboard-test.o
#out/piece-test.o: engine/piece.cpp engine/piece.h engine/enums.h
#	g++ $(TEST_FLAG) engine/piece.cpp -c -o out/piece-test.o
#out/position-test.o: engine/position.cpp engine/position.h engine/direction.h engine/enums.h
#	g++ $(TEST_FLAG) engine/position.cpp -c -o out/position-test.o
#
# 
#
#out/build_from_graph.o: graph_board/build_from_graph.cpp graph_board/build_from_graph.h engine/board.h graph_board/graph_board.h
#	g++  graph_board/build_from_graph.cpp -c -o out/build_from_graph.o
#out/graph_board.o: graph_board/graph_board.cpp graph_board/graph_board.h 
#	g++  graph_board/graph_board.cpp -c -o out/graph_board.o
#
#out/build_from_graph.so: graph_board/build_from_graph.cpp graph_board/build_from_graph.h engine/board.h graph_board/graph_board.h
#	g++ -fPIC graph_board/build_from_graph.cpp -c -o out/build_from_graph.so
#out/graph_board.so: graph_board/graph_board.cpp graph_board/graph_board.h
#	g++ -fPIC graph_board/graph_board.cpp -c -o out/graph_board.so
#out/update_board.so: graph_board/update_board.cpp graph_board/update_board.h engine/board.h graph_board/graph_board.h
#	g++ -fPIC graph_board/update_board.cpp -c -o out/update_board.o
#
# 
#
#out/build_from_graph-test.o: graph_board/build_from_graph.cpp graph_board/build_from_graph.h engine/board.h graph_board/graph_board.h
#	g++ $(TEST_FLAG)  graph_board/build_from_graph.cpp -c -o out/build_from_graph-test.o
#out/graph_board-test.o: graph_board/graph_board.cpp graph_board/graph_board.h 
#	g++ $(TEST_FLAG)  graph_board/graph_board.cpp -c -o out/graph_board-test.o
#out/update_board-test.o: graph_board/update_board.cpp graph_board/update_board.h engine/board.h graph_board/graph_board.h
#	g++ $(TEST_FLAG) graph_board/update_board.cpp -c -o out/update_board-test.o
#out/embadded_board-test.o: embadded_board.cpp embadded_board.h graph_board/update_board.h engine/board.h graph_board/graph_board.h
#	g++ $(TEST_FLAG) embadded_board.cpp -c -o out/embadded_board-test.o
#
#
#
#graph_elements := out/build_from_graph.o out/graph_board.o out/update_board.o
#graph_elements_shared := out/build_from_graph.so out/graph_board.so out/update_board.so
#engine_elements_shared := out/position.so out/action.so out/board.so out/direction.so out/enums.so out/gameboard.so out/piece.so
#engine_elements := out/position.o out/action.o out/direction.o out/board.o out/enums.o out/gameboard.o out/piece.o
#
#test_elements := out/build_from_graph-test.o out/graph_board-test.o out/update_board-test.o out/embadded_board-test.o\
#				 out/position-test.o out/action-test.o out/direction-test.o out/board-test.o out/enums-test.o out/gameboard-test.o out/piece-test.o
#
#
#
#
#test: $(test_elements) test.cpp
#	g++ $(TEST_FLAG) test.cpp $(test_elements) -o test
#
#test-fast: $(test_elements_fast) test.cpp
#	g++ $(TEST_FAST_FLAG) test.cpp $(test_elements_fast) -o test-fast
#
#engine.so : $(engine_elements_shared) $(graph_elements_shared) interface.cpp
#	g++ -fPIC -shared -o engine.so interface.cpp $(graph_elements_shared) $(engine_elements_shared)
#				
#
#interface.so :  interface.cpp engine/*.cpp engine/*.h embadded_board.* graph_board/*
#	g++ engine/*.cpp engine/*.h graph_board/* embadded_board.* interface.cpp -shared -O3  -fPIC -o interface.so
#clean:
#	rm out/*
