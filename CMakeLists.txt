cmake_minimum_required(VERSION 3.10)
project(stalink CXX)

message("proj source dir: ${PROJECT_SOURCE_DIR}")

find_package(Boost REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
else()
  message(FATAL_ERROR "Boost not found!")
endif(Boost_FOUND)

if(NOT DEFINED YAS_SEARCH_PATH)
    find_package(Git QUIET)
    if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
        option(GIT_SUBMODULE "Check submodules during build" ON)
        if(GIT_SUBMODULE)
        message(STATUS "YAS submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
        endif()
    endif()

    set(YAS_SEARCH_PATH ${PROJECT_SOURCE_DIR}/external/yas/include)
endif()

message("YAS_SEARCH_PATH=${YAS_SEARCH_PATH}")

file(GLOB_RECURSE SRC_SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
    ${YAS_SEARCH_PATH}
    ${Boost_INCLUDE_DIR}
)

if(NOT DEFINED STALINK_BUILD_STATIC)
    set(STALINK_BUILD_STATIC 1)
endif()


add_library(stalink-static STATIC ${SRC_SOURCES})
add_library(stalink SHARED ${SRC_SOURCES})


install(TARGETS stalink-static stalink
        DESTINATION lib)

install(DIRECTORY src/channel
        DESTINATION include
        FILES_MATCHING PATTERN "*.hpp")
install(DIRECTORY src/client
        DESTINATION include
        FILES_MATCHING PATTERN "*.hpp")
install(DIRECTORY src/common
        DESTINATION include
        FILES_MATCHING PATTERN "*.hpp")
install(DIRECTORY src/server
        DESTINATION include
        FILES_MATCHING PATTERN "*.hpp")



