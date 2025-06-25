cmake_minimum_required(VERSION 3.15)
project(ProgettoHive LANGUAGES CXX)

# -----------------------------------------------------------------------------
# 1) Compiler settings
# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# -----------------------------------------------------------------------------
# 2) Locate LibTorch
# -----------------------------------------------------------------------------
# Pass -DCMAKE_PREFIX_PATH=/path/to/libtorch when you invoke CMake if needed.
find_package(Torch REQUIRED)
message(STATUS "Torch include dirs: ${TORCH_INCLUDE_DIRS}")
message(STATUS "Torch libraries:   ${TORCH_LIBRARIES}")

# -----------------------------------------------------------------------------
# 3) Locate Python3 (development headers + library)
# -----------------------------------------------------------------------------
find_package(Python3 REQUIRED COMPONENTS Development)
message(STATUS "Python3 include dirs: ${Python3_INCLUDE_DIRS}")
message(STATUS "Python3 libraries:    ${Python3_LIBRARIES}")

# -----------------------------------------------------------------------------
# 4) engine static library
# -----------------------------------------------------------------------------
add_library(engine STATIC
    engine/action.cpp
    engine/board.cpp
    engine/direction.cpp
    engine/enums.cpp
    engine/gameboard.cpp
    engine/piece.cpp
    engine/position.cpp
)
target_include_directories(engine PUBLIC
    ${CMAKE_SOURCE_DIR}/engine
)

# -----------------------------------------------------------------------------
# 5) embadded_board static library
# -----------------------------------------------------------------------------
add_library(embadded_board STATIC
    embadded_board.cpp
)
target_include_directories(embadded_board PUBLIC
    ${CMAKE_SOURCE_DIR}
)
target_link_libraries(embadded_board PUBLIC engine)

# -----------------------------------------------------------------------------
# 6) interface_module static library
# -----------------------------------------------------------------------------
add_library(interface_module STATIC
    interface.cpp
)
target_include_directories(interface_module PUBLIC
    ${CMAKE_SOURCE_DIR}
)
target_link_libraries(interface_module PUBLIC engine)

# -----------------------------------------------------------------------------
# 7) mcts shared library (.so)
# -----------------------------------------------------------------------------
add_library(mcts SHARED
    mcts.cpp
    node.cpp
)

# Make sure all headers (root, engine, Torch, Python) are visible
target_include_directories(mcts PUBLIC
    ${CMAKE_SOURCE_DIR}            # for mcts.h, node.h
    ${CMAKE_SOURCE_DIR}/engine     # for engine/*.h
    ${TORCH_INCLUDE_DIRS}          # for torch headers
    ${Python3_INCLUDE_DIRS}        # for Python.h
)

# Link against your static libs + Torch + Python
target_link_libraries(mcts PUBLIC
    engine
    embadded_board
    interface_module
    ${TORCH_LIBRARIES}
    ${Python3_LIBRARIES}
)

# Required for building PyTorch C++ extensions
target_compile_definitions(mcts PUBLIC TORCH_API_INCLUDE_EXTENSION_H)

# Strip the default “lib” prefix so it becomes exactly mcts.so
set_target_properties(mcts PROPERTIES
    PREFIX ""
    OUTPUT_NAME "mcts"
)

# -----------------------------------------------------------------------------
# 8) (Optional) install rules
# -----------------------------------------------------------------------------
install(TARGETS mcts LIBRARY DESTINATION lib)
install(DIRECTORY engine/             DESTINATION include/engine  FILES_MATCHING PATTERN "*.h")
install(FILES embadded_board.h interface.h mcts.h node.h DESTINATION include)
