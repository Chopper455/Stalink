cmake_minimum_required(VERSION 3.10)
project(stalink CXX)

message("proj source dir: ${PROJECT_SOURCE_DIR}")

find_package(Boost REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
else()
  message(FATAL_ERROR "Boost not found!")
endif(Boost_FOUND)

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

find_path(YAS_INCLUDE_DIR "version.hpp" PATHS ${PROJECT_SOURCE_DIR}/external/yas/include/yas)

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/external/yas/include/yas/version.hpp")
	message(FATAL_ERROR "The YAS submodule wasn't downloaded!")
endif()

file(GLOB_RECURSE SRC_SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}/src/
	${YAS_INCLUDE_DIR}/..
	${Boost_INCLUDE_DIR}
)

add_library(stalink STATIC ${SRC_SOURCES})
