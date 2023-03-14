cmake_minimum_required(VERSION 3.22)
project(Obelisk)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
file(GLOB_RECURSE CPP_HEADERS include/*.h include/*.hpp)
file(GLOB_RECURSE CPP_SOURCES source/*.cpp source/*.cc)
file(GLOB_RECURSE CPP_TESTS tests/*.cpp)

set(CMAKE_CXX_STANDARD 23)

find_package(Boost REQUIRED COMPONENTS unit_test_framework url)
find_package(OpenSSL REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(include)

add_executable(Obelisk ${CPP_HEADERS} ${CPP_SOURCES})
target_link_libraries(Obelisk PRIVATE OpenSSL::SSL OpenSSL::Crypto Boost::url)

target_compile_options(Obelisk
        PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/bigobj>
        $<$<CXX_COMPILER_ID:GNU>:-Wa,-mbig-obj>)
