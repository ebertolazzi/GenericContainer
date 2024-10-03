cmake_minimum_required(VERSION 3.5)
project("reentrant-variant-cpp")

## Flex/Bison configuration

# On macOS, search Homebrew for keg-only versions of Bison and Flex. Xcode does
# not provide new enough versions for us to use.
if (CMAKE_HOST_SYSTEM_NAME MATCHES "Darwin")
    execute_process(
        COMMAND brew --prefix bison
        RESULT_VARIABLE BREW_BISON
        OUTPUT_VARIABLE BREW_BISON_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if (BREW_BISON EQUAL 0 AND EXISTS "${BREW_BISON_PREFIX}")
        message(STATUS "Found Bison keg installed by Homebrew at ${BREW_BISON_PREFIX}")
        set(BISON_EXECUTABLE "${BREW_BISON_PREFIX}/bin/bison")
    endif()

    execute_process(
        COMMAND brew --prefix flex
        RESULT_VARIABLE BREW_FLEX
        OUTPUT_VARIABLE BREW_FLEX_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if (BREW_FLEX EQUAL 0 AND EXISTS "${BREW_FLEX_PREFIX}")
        message(STATUS "Found Flex keg installed by Homebrew at ${BREW_FLEX_PREFIX}")
        set(FLEX_EXECUTABLE "${BREW_FLEX_PREFIX}/bin/flex")
    endif()
endif()

find_package(BISON 3.8 REQUIRED)
find_package(FLEX REQUIRED)

if (WIN32)
    set (ADDITIONAL_FLEX_FLAGS "--wincompat")
else()
    set (ADDITIONAL_FLEX_FLAGS "")
endif()
FLEX_TARGET(LEXER ${CMAKE_CURRENT_SOURCE_DIR}/lexer.l
${CMAKE_CURRENT_SOURCE_DIR}/lexer.cpp DEFINES_FILE ${CMAKE_CURRENT_SOURCE_DIR}/lexer.hpp COMPILE_FLAGS ${ADDITIONAL_FLEX_FLAGS})
BISON_TARGET(PARSER ${CMAKE_CURRENT_SOURCE_DIR}/parser.y
${CMAKE_CURRENT_BINARY_DIR}/parser.cpp COMPILE_FLAGS "-d -v")
ADD_FLEX_BISON_DEPENDENCY(LEXER PARSER)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB source_files
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_executable(${PROJECT_NAME} ${source_files} ${BISON_PARSER_OUTPUTS} ${FLEX_LEXER_OUTPUTS})