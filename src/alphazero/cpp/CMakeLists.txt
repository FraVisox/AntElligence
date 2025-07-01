cmake_minimum_required(VERSION 3.15)
project(ProgettoHive LANGUAGES CXX)

# -----------------------------------------------------------------------------
# 1) Compiler settings
# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -g -O0")
# -----------------------------------------------------------------------------
# 2) Find LibTorch & Python3 dev
# -----------------------------------------------------------------------------
# Pass -DCMAKE_PREFIX_PATH=/path/to/libtorch when you invoke cmake if needed
find_package(Torch REQUIRED)
find_package(Python3 COMPONENTS Development REQUIRED)

message(STATUS "Torch include dirs:    ${TORCH_INCLUDE_DIRS}")
message(STATUS "Torch libraries:      ${TORCH_LIBRARIES}")
message(STATUS "Python3 include dirs: ${Python3_INCLUDE_DIRS}")
message(STATUS "Python3 libraries:    ${Python3_LIBRARIES}")

# -----------------------------------------------------------------------------
# 3) Engine static library (includes embadded_board)
# -----------------------------------------------------------------------------
file(GLOB ENGINE_SOURCES
    "${CMAKE_SOURCE_DIR}/engine/*.cpp"
)
add_library(engine STATIC ${ENGINE_SOURCES})
target_include_directories(engine PUBLIC
    ${CMAKE_SOURCE_DIR}/engine
)

# -----------------------------------------------------------------------------
# 4) MCTS static library
#    - depends only on engine + LibTorch + Python
# -----------------------------------------------------------------------------
add_library(mcts STATIC
    mcts/node.cpp
    mcts/mcts.cpp
)
target_include_directories(mcts PUBLIC
    ${CMAKE_SOURCE_DIR}/mcts
    ${CMAKE_SOURCE_DIR}/engine
    ${TORCH_INCLUDE_DIRS}
    ${Python3_INCLUDE_DIRS}
)
target_link_libraries(mcts PUBLIC
    engine
    ${TORCH_LIBRARIES}
    Python3::Python
)
target_compile_definitions(mcts PUBLIC
    TORCH_API_INCLUDE_EXTENSION_H
)

# -----------------------------------------------------------------------------
# 5) Interface shared library (interface.so)
#    - now depends on mcts (and transitively on engine)
# -----------------------------------------------------------------------------
add_library(interface SHARED
    interface.cpp
)
target_include_directories(interface PUBLIC
    ${CMAKE_SOURCE_DIR}        # for interface.h
    ${CMAKE_SOURCE_DIR}/mcts   # if you include mcts/*.h in interface.cpp
)
target_link_libraries(interface PUBLIC
    mcts
    engine
)
# remove automatic "lib" prefix so output is exactly interface.so
set_target_properties(interface PROPERTIES
    PREFIX ""
    OUTPUT_NAME "interface"
)

# -----------------------------------------------------------------------------
# 5.1) Interface shared library (interface_reduced.so)
#    - now depends on mcts (and transitively on engine)
# -----------------------------------------------------------------------------
add_library(interface_reduced SHARED
interface_reduced.cpp
)
target_include_directories(interface_reduced PUBLIC
    ${CMAKE_SOURCE_DIR}        # for interface.h
)
target_link_libraries(interface_reduced PUBLIC
    engine
)

# remove automatic "lib" prefix so output is exactly interface.so
set_target_properties(interface_reduced PROPERTIES
    PREFIX ""
    OUTPUT_NAME "interface_reduced"
)

# -----------------------------------------------------------------------------
# 6) (Optional) install rules
# -----------------------------------------------------------------------------
install(TARGETS interface LIBRARY DESTINATION lib)
install(DIRECTORY engine/      DESTINATION include/engine  FILES_MATCHING PATTERN "*.h")
install(FILES interface.h             DESTINATION include)
install(DIRECTORY mcts/      DESTINATION include/mcts    FILES_MATCHING PATTERN "*.h")


# at the bottom of CMakeLists.txt

# 7) test executable
add_executable(test_app
    self-play.cpp
)

# make sure the headers from engine/, mcts/, and project root are visible:
target_include_directories(test_app PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/engine
    ${CMAKE_SOURCE_DIR}/mcts
)

# link against your shared interface.so (and transitively mcts, engine, Torch & Python)
target_link_libraries(test_app PRIVATE
    interface
    ${TORCH_LIBRARIES}
    Python3::Python
)

# if your Torch install isn’t in a standard path:
# you already passed -DCMAKE_PREFIX_PATH=/path/to/libtorch to find_package(Torch)
