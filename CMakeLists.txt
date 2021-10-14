############################################################################
#                                                                          #
#  file: CMakeLists.txt                                                    #
#  Copyright (C) 2016                                                      #
#      Enrico Bertolazzi                                                   #
#      Dipartimento di Ingegneria Industriale                              #
#      Universita` degli Studi di Trento                                   #
#      email: enrico.bertolazzi@ing.unitn.it                               #
#                                                                          #
############################################################################

cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_INSTALL_MESSAGE ALWAYS)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project(GenericContainer 
  VERSION 1.0.0
  HOMEPAGE_URL "https://ebertolazzi.github.io/GenericContainer/")

# NOTE: Predictable target name
set(STATIC_PROJECT "${PROJECT_NAME}Static")
set(DYNAMIC_PROJECT "${PROJECT_NAME}")

# FIXME: This should refer to functions that acts on targets
include(./CMakeLists-cflags.txt)
include(./CMakeUtilities.txt)

# Evaluating the suffix to append to target built by this CMake file
ebertolazzi_artifacts_suffix(ARTIFACTS_STATIC_SUFFIX TRUE)
if (BUILD_SHARED)
  ebertolazzi_artifacts_suffix(ARTIFACTS_DYNAMIC_SUFFIX FALSE)
endif()

message(STATUS "Compiler used: ${CMAKE_CXX_COMPILER_ID}")
if (APPLE)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLUA_USE_MACOSX")
elseif(LINUX OR UNIX)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLUA_USE_LINUX")
else()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
endif()

# Creating Dependency libraries
# RAPIDJSON LIBRARY
set(rapidjson_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src_json_interface")
file(GLOB_RECURSE rapidjson_HEADERS "${rapidjson_DIR}/*.h")
add_library(rapidjson INTERFACE)
# target_include_directories(rapidjson INTERFACE ${rapidjson_DIR})
set_target_properties(rapidjson PROPERTIES
  PUBLIC_HEADER "${rapidjson_HEADERS}")

# LUA LIBRARY
set(lualib_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src_lua_interface/lua-5.4.3/src")
file(GLOB_RECURSE lualib_HEADERS "${lualib_DIR}/*.h")
file(GLOB lualib_SOURCES "${lualib_DIR}/*.c")
add_library(lualib STATIC ${lualib_SOURCES})
target_include_directories(lualib PRIVATE ${lualib_DIR})
target_link_libraries(lualib PRIVATE ${CMAKE_DL_LIBS})
set_target_properties(lualib PROPERTIES 
  PUBLIC_HEADER "${lualib_HEADERS}"
  POSITION_INDEPENDENT_CODE ON)

set(SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SOURCES_JSON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src_json_interface")
set(SOURCES_LUA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src_lua_interface")

# NOTE: Glob shall be avoided. Headers and sources shall be declared 
# manually in order to rebuild cmake-cache automatically.
# Ref: https://cmake.org/cmake/help/v3.15/command/file.html?highlight=glob#filesystem
set(SOURCES
  "${SOURCES_DIR}/GenericContainer.cc"
  "${SOURCES_DIR}/GenericContainerCinterface.cc"
  "${SOURCES_DIR}/GenericContainerSerialize.cc"
  "${SOURCES_DIR}/GenericContainerSupport.cc"
  "${SOURCES_DIR}/GenericContainerTables.cc")
set(SOURCES
  ${SOURCES}
  "${SOURCES_JSON_DIR}/GenericContainerJson"
  "${SOURCES_JSON_DIR}/GenericContainerJsonHandler")
set(SOURCES
  ${SOURCES}
  "${SOURCES_LUA_DIR}/GenericContainerLuaInterface.cc"
  "${SOURCES_LUA_DIR}/GenericContainerLuaPmain.cc")

  # FIXME: I had to invent a workaroud to have all the headers... 
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src*/*.hh")

add_library(${STATIC_PROJECT} STATIC
  ${SOURCES})
target_include_directories(${STATIC_PROJECT} PUBLIC
  "$<BUILD_INTERFACE:${SOURCES_DIR}>"
  "$<BUILD_INTERFACE:${SOURCES_JSON_DIR}>"
  "$<BUILD_INTERFACE:${SOURCES_LUA_DIR}>"
  "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>")
target_include_directories(${STATIC_PROJECT} PRIVATE lualib)
target_link_libraries(${STATIC_PROJECT} PUBLIC lualib)
target_link_libraries(${STATIC_PROJECT} PUBLIC rapidjson)
set_target_properties(${STATIC_PROJECT} PROPERTIES
  OUTPUT_NAME "${PROJECT_NAME}${ARTIFACTS_STATIC_SUFFIX}"
  PUBLIC_HEADER "${HEADERS}"
  POSITION_INDEPENDENT_CODE ON)  

if(BUILD_SHARED)
  add_library(${DYNAMIC_PROJECT} SHARED
    ${SOURCES})
  set_target_properties(${DYNAMIC_PROJECT} PROPERTIES
    OUTPUT_NAME "${PROJECT_NAME}${ARTIFACTS_DYNAMIC_SUFFIX}"
    PUBLIC_HEADER "${HEADERS}"
    POSITION_INDEPENDENT_CODE ON)
  target_include_directories(${DYNAMIC_PROJECT} PRIVATE
    ${SOURCES_DIR} ${SOURCES_JSON_DIR} ${SOURCES_LUA_DIR})
  target_link_libraries(${DYNAMIC_PROJECT} PRIVATE rapidjson lualib)
  if(WIN32)
    set_target_properties(${DYNAMIC_PROJECT} PROPERTIES 
      WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
  endif()
  target_link_libraries(${DYNAMIC_PROJECT} PRIVATE ${CMAKE_DL_LIBS})
endif()

# INSTALL( FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include ) # FIXME

if(BUILD_EXECUTABLE)
  enable_testing()
  set(TEST_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin")

  add_executable(example1 examples/example1.cc)
  target_link_libraries(example1 ${STATIC_PROJECT})
  set_target_properties(example1 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${TEST_OUTPUT_PATH}")
  add_test(example1 ${TEST_OUTPUT_PATH}/example1)

  add_executable(example2 examples/example2.cc)
  target_link_libraries(example2 ${STATIC_PROJECT})
  set_target_properties(example2 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${TEST_OUTPUT_PATH}")
  add_test(example2 ${TEST_OUTPUT_PATH}/example2)

  add_executable(example3 examples/example3.cc)
  target_link_libraries(example3 ${STATIC_PROJECT})
  set_target_properties(example3 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${TEST_OUTPUT_PATH}")
  add_test(example3 ${TEST_OUTPUT_PATH}/example3)

  add_executable(example4 examples/example4.cc)
  target_link_libraries(example4 ${STATIC_PROJECT})
  set_target_properties(example4 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${TEST_OUTPUT_PATH}")
  add_test(example4 ${TEST_OUTPUT_PATH}/example4)

  add_executable(example5 examples/example5.cc)
  target_link_libraries(example5 ${STATIC_PROJECT})
  set_target_properties(example5 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${TEST_OUTPUT_PATH}")
  add_test(example5 ${TEST_OUTPUT_PATH}/example5)

  add_executable(example6 examples/example6.c) # IT'S C!
  target_link_libraries(example6 ${STATIC_PROJECT})
  set_target_properties(example6 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${TEST_OUTPUT_PATH}")
  add_test(example6 ${TEST_OUTPUT_PATH}/example6)

  add_executable(example7 examples/example7.cc)
  target_link_libraries(example7 ${STATIC_PROJECT})
  set_target_properties(example7 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${TEST_OUTPUT_PATH}")
  add_test(example7 ${TEST_OUTPUT_PATH}/example7)

  add_executable(example8 examples/example8.cc)
  target_link_libraries(example8 ${STATIC_PROJECT})
  set_target_properties(example8 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${TEST_OUTPUT_PATH}")
  add_test(example8 ${TEST_OUTPUT_PATH}/example8)

  add_executable(example9 examples/example9.cc)
  target_link_libraries(example9 ${STATIC_PROJECT})
  set_target_properties(example9 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${TEST_OUTPUT_PATH}")
  add_test(example9 ${TEST_OUTPUT_PATH}/example9)

  add_executable(JSON_test_1 src_json_interface/tests/JSON_test_1.cc)
  target_link_libraries(JSON_test_1 ${STATIC_PROJECT})
  set_target_properties(JSON_test_1 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${TEST_OUTPUT_PATH}")
  add_test(JSON_test_1 ${TEST_OUTPUT_PATH}/JSON_test_1)

  add_executable(example_lua1 src_lua_interface/tests/example_lua1.cc)
  target_link_libraries(example_lua1 ${STATIC_PROJECT} lualib)
  set_target_properties(example_lua1 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${TEST_OUTPUT_PATH}")
  add_test(example_lua1 ${TEST_OUTPUT_PATH}/example_lua1)

  add_executable(example_lua2 src_lua_interface/tests/example_lua2.cc)
  target_link_libraries(example_lua2 ${STATIC_PROJECT} lualib)
  set_target_properties(example_lua2 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${TEST_OUTPUT_PATH}")
  add_test(example_lua2 ${TEST_OUTPUT_PATH}/example_lua2)

  add_executable(example_lua3 src_lua_interface/tests/example_lua3.cc)
  target_link_libraries(example_lua3 ${STATIC_PROJECT} lualib)
  set_target_properties(example_lua3 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${TEST_OUTPUT_PATH}")
  add_test(example_lua3 ${TEST_OUTPUT_PATH}/example_lua3)
endif()

# INSTALL TARGETS

# FIXME: CMakeLists should not override CMAKE_INSTALL_PREFIX
# ref: https://gitlab.kitware.com/cmake/cmake/-/issues/19322
# SET( CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/lib )
# Also I realy don't like the solution I came up with...
if(NOT ADDITIONAL_PATH_PREFIX)
  set(ADDITIONAL_PATH_PREFIX "lib")
endif()

set(INSTALL_TARGETS ${STATIC_PROJECT})
if(BUILD_SHARED)
set(INSTALL_TARGETS ${INSTALL_TARGETS} ${DYNAMIC_PROJECT})
endif()
# Current project scope target
install(TARGETS ${INSTALL_TARGETS}
  RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/${ADDITIONAL_PATH_PREFIX}/bin"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/${ADDITIONAL_PATH_PREFIX}/lib"
  LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/${ADDITIONAL_PATH_PREFIX}/dll"
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_PREFIX}/${ADDITIONAL_PATH_PREFIX}/include/${PROJECT_NAME}"
  PRIVATE_HEADER DESTINATION "${CMAKE_INSTALL_PREFIX}/${ADDITIONAL_PATH_PREFIX}/include/${PROJECT_NAME}")
# Rapidjson target (exposed in public api)
install(TARGETS rapidjson
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_PREFIX}/${ADDITIONAL_PATH_PREFIX}/include/rapidjson")
install(TARGETS lualib
  RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/${ADDITIONAL_PATH_PREFIX}/bin"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/${ADDITIONAL_PATH_PREFIX}/lib"
  LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/${ADDITIONAL_PATH_PREFIX}/dll"
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_PREFIX}/${ADDITIONAL_PATH_PREFIX}/include/lua")

message(STATUS "C compiler                  = ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER}")
message(STATUS "C++ compiler                = ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE     = ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "CMAKE_CXX_FLAGS             = ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_C_FLAGS_RELEASE       = ${CMAKE_C_FLAGS_RELEASE}")
message(STATUS "CMAKE_C_FLAGS               = ${CMAKE_C_FLAGS}")
message(STATUS "CMAKE_SYSTEM_NAME           = ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR      = ${CMAKE_SYSTEM_PROCESSOR}")
if(APPLE)
  message(STATUS "CMAKE_OSX_SYSROOT           = ${CMAKE_OSX_SYSROOT}")
  message(STATUS "CMAKE_OSX_ARCHITECTURES     = ${CMAKE_OSX_ARCHITECTURES}")
  message(STATUS "CMAKE_OSX_DEPLOYMENT_TARGET = ${CMAKE_OSX_DEPLOYMENT_TARGET}")
endif()
message(STATUS "SOURCES = ${SOURCES}")
message(STATUS "HEADERS = ${HEADERS}")
