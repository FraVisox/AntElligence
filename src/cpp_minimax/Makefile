# Compiler and flags
CXX = g++ -Ofast    -fopenmp -funroll-loops -fomit-frame-pointer -fno-rtti -mavx512bw -march=native -flto -fno-exceptions # -mavx512vbmi2    

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

# Perft uses a portable build: the compat/ shim shadows <immintrin.h> with
# pure-C++ equivalents, so no AVX-512 hardware is required at runtime.
# Only the engine sources are linked (minimax/TT/engine are not needed).
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
  OMP_FLAG = -Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include -L/opt/homebrew/opt/libomp/lib -lomp
else
  OMP_FLAG = -fopenmp
endif
CXX_PERFT  = g++ -O3 -std=c++17 -I./compat $(OMP_FLAG)
PERFT_SRCS = $(ENGINE_CPPS)

perft : perft.cpp $(PERFT_SRCS) $(ENGINE_H) compat/immintrin.h
	$(CXX_PERFT) perft.cpp $(PERFT_SRCS) -o perft



clean:
	rm -f $(OUT_DIR)/*.o 
 
