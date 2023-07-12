cmake_minimum_required(VERSION 3.21)
project(obelisk)
set(VERSION 1.0.0)
file(GLOB_RECURSE CPP_HEADERS include/*.h include/*.hpp)
file(GLOB_RECURSE CPP_SOURCES source/*.cpp source/*.cc)

set(CMAKE_CXX_STANDARD 23)

find_package(Boost REQUIRED COMPONENTS url thread json fiber)
find_package(fmt CONFIG REQUIRED)
find_package(sahara REQUIRED)

add_library(${PROJECT_NAME} ${CPP_HEADERS} ${CPP_SOURCES} )
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})

configure_package_config_file(cmake/template/obelisk.in.cmake
        ${PROJECT_NAME}-config.cmake
        INSTALL_DESTINATION ./)
write_basic_package_version_file(
        ${PROJECT_NAME}-config-version.cmake
        VERSION ${VERSION}
        COMPATIBILITY ExactVersion)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
        DESTINATION share/${PROJECT_NAME})
install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME} FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
install(EXPORT ${PROJECT_NAME}-targets NAMESPACE ${PROJECT_NAME}:: DESTINATION share/${PROJECT_NAME})
#target_link_libraries(obelisk PRIVATE )
#
#file(GLOB_RECURSE TEST_HEADERS test/*.h test/*.hpp)
#file(GLOB_RECURSE TEST_SOURCES test/*.cpp test/*.cc)
#add_executable(obelisk_test ${TEST_HEADERS} ${TEST_SOURCES} )
#target_link_libraries(obelisk_test PRIVATE ws2_32 obelisk ws2_32 Boost::json Boost::locale sahara fmt::fmt)


