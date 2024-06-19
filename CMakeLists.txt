cmake_minimum_required(VERSION 3.10)
project(OS_Scheduler_Simulator)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS core base)

include(${wxWidgets_USE_FILE})

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include ${wxWidgets_INCLUDE_DIRS})

# Source files
file(GLOB SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Create executable
add_executable(OS_Scheduler_Simulator ${SRC_FILES})

# Link wxWidgets libraries
target_link_libraries(OS_Scheduler_Simulator ${wxWidgets_LIBRARIES})
