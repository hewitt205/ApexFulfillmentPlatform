cmake_minimum_required(VERSION 3.20)
project(ApexFulfillmentPlatform CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug)
endif()

add_library(apex_coordination
    src/coordination/ServiceRegistry.cpp
    src/coordination/LeaderElection.cpp
)
target_include_directories(apex_coordination PUBLIC src)

add_executable(apex_demo src/main.cpp)
target_link_libraries(apex_demo PRIVATE apex_coordination)

enable_testing()
add_subdirectory(tests)
