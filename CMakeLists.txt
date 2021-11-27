cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(ConfigFile VERSION 2.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_TEST_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/test)

set(EXTRA_OBJS
    ${PROJECT_SOURCE_DIR}/../Tools/build/CMakeFiles/cpp_tools.dir/source/common.cpp.o
    ${PROJECT_SOURCE_DIR}/../Tools/build/CMakeFiles/cpp_tools.dir/source/trim.cpp.o
    ${PROJECT_SOURCE_DIR}/../tinyxml2/libtinyxml2.a
)

configure_file(ConfigFileInfo.h.in ${PROJECT_SOURCE_DIR}/include/ConfigFileInfo.h)

# Define ConfigFile library to be used externally
add_library(configFile source/ConfigFile.cpp )
install(TARGETS configFile DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} )

target_include_directories(configFile PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/../tinyxml2  ${PROJECT_SOURCE_DIR}/../Tools/include  )


find_library(cppTools cpp_tools PATHS ${PROJECT_SOURCE_DIR}/../Tools/bin)

foreach(sourceFile
  test/tConfigFile.cpp
  test/TestReadConfigData.cpp
)
    # Use string replace to cut off '.cpp'
    string(REPLACE ".cpp" "" target ${sourceFile} )
    string(REPLACE "test/" "" target ${target} )
    add_executable(${target} ${sourceFile} )
    target_include_directories(${target} PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/../tinyxml2
        ${PROJECT_SOURCE_DIR}/../Tools/include )
    target_link_libraries(${target} PUBLIC configFile ${EXTRA_OBJS} )
    install(TARGETS ${target} DESTINATION ${CMAKE_TEST_OUTPUT_DIRECTORY} )
endforeach(sourceFile "")
