# initializing stuff
cmake_minimum_required(VERSION 3.11)
project(Claritaxx VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

# all of the main typical files, used for all builds
set(Common_SRC_Files 
src/board.cpp)

# Build
add_executable(Claritaxx_Native ${Common_SRC_Files} src/bmi2.cpp src/uci.cpp)

target_compile_options(Claritaxx_Native PRIVATE -Wall -Wextra -Werror -Wpedantic -march=native -flto)

target_link_libraries(Claritaxx_Native -static)

# might need this line again if nnue
#target_compile_definitions(Clarity_Native PUBLIC NetworkFile="../src/nets/clarity_net007.nnue")
