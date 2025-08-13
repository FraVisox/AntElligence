# Compiler and flags
CXX = g++ -Ofast    -fopenmp -funroll-loops -fomit-frame-pointer -fno-rtti -mavx512bw # -mavx512vbmi2  -march=native -flto #  -fno-exceptions 

#-fsanitize=address
OUT_DIR = out

# Sources
SRC_ENGINE = action board direction enums gameboard piece position board_bitset weights dynamic_eval

# Extensions
ENGINE_OBJS = $(addprefix $(OUT_DIR)/, $(addsuffix .o, $(SRC_ENGINE))) 
ENGINE_CPPS = $(addprefix engine/, $(addsuffix .cpp, $(SRC_ENGINE)))
ENGINE_H = $(addprefix engine/, $(addsuffix .h, $(SRC_ENGINE)))


ALL_OJB = $(ENGINE_OBJS) out/engine.o out/minimax.o   out/TT.o 
# Default targetvva
all: $(ENGINE_OBJS)

out/minimax.o : minimax.cpp $(ENGINE_H)
	$(CXX)  $(CXXFLAGS) -c $< -o $@
 
out/engine.o : engine.cpp $(ENGINE_H) 
	$(CXX) $(CXXFLAGS) -c $< -o $@

out/TT.o : TT.cpp $(ENGINE_H) TT.h 
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OUT_DIR)/board.o: engine/board.cpp $(ENGINE_H) 
	$(CXX) $(CXXFLAGS) -c $< -o $@



# Pattern rule for object files
$(OUT_DIR)/%.o: engine/%.cpp $(ENGINE_H)
	$(CXX) $(CXXFLAGS) -c $< -o $@


# Build final targets
test-% : test/test-%.cpp $(ALL_OJB)  
	$(CXX) $(CXXFLAGS) test/$@.cpp $(ALL_OJB) -o test/$@


main : main.cpp $(ALL_OJB)  
	$(CXX) $(CXXFLAGS) main.cpp $(ALL_OJB) -o main

self_play : self_play.cpp $(ALL_OJB)  
	$(CXX) $(CXXFLAGS) self_play.cpp $(ALL_OJB) -o self_play



clean:
	rm -f $(OUT_DIR)/*.o 
 
