cmake_minimum_required(VERSION 3.20)
project(CppHttpServer VERSION 1.0 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  add_compile_options(-O3 -march=native -DNDEBUG)
endif()


find_package(Boost 1.81 REQUIRED COMPONENTS system)

include(FetchContent)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(json)


add_executable(server
    src/main.cpp
    src/async_server.cpp
)

target_include_directories(server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(server PRIVATE
    Boost::system
    nlohmann_json::nlohmann_json
    pthread          
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(server PRIVATE
    -Wall -Wextra -Wpedantic -fsanitize=address,undefined
  )
  target_link_options(server PRIVATE -fsanitize=address,undefined)
endif()
