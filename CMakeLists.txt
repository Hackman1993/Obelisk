cmake_minimum_required(VERSION 3.22)
project(obelisk)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
file(GLOB_RECURSE CPP_HEADERS include/*.h include/*.hpp)
file(GLOB_RECURSE CPP_SOURCES source/*.cpp source/*.cc)
file(GLOB_RECURSE CPP_TESTS tests/*.cpp)

set(CMAKE_CXX_STANDARD 23)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")

find_package(Boost REQUIRED COMPONENTS unit_test_framework fiber url thread)
find_package(OpenSSL REQUIRED)
find_package(cryptopp CONFIG REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(include)
include_directories(D:/Workspace/Cpp/Rosetta/include)

include_directories("C:\\Program Files\\MySQL\\MySQL Connector C++ 8.0\\include")
link_directories("C:/Program Files/MySQL/MySQL Connector C++ 8.0/debug/vs14")
if (${CMAKE_BUILD_TYPE} MATCHES Debug)
    link_directories(D:/Workspace/Cpp/Rosetta/cmake-build-debug)
elseif (Release)
    link_directories(D:/Workspace/Cpp/Rosetta/cmake-build-release)
endif()

add_definitions(${__cpp_impl_coroutine})
add_library(obelisk ${CPP_HEADERS} ${CPP_SOURCES})
target_link_libraries(obelisk PRIVATE OpenSSL::SSL OpenSSL::Crypto Boost::fiber Boost::url Boost::thread)
#target_compile_options(obelisk PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/bigobj>> $<$<CXX_COMPILER_ID:GNU>:-Wa -std=c++20>)
target_compile_features(obelisk PUBLIC cxx_std_20)

file(GLOB_RECURSE TEST_HEADERS test/*.h test/*.hpp)
file(GLOB_RECURSE TEST_SOURCES test/*.cpp test/*.cc)
add_executable(obelisk_test ${TEST_HEADERS} ${TEST_SOURCES} )
target_link_libraries(obelisk_test PRIVATE obelisk cryptopp::cryptopp)


