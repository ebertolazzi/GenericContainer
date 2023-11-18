############################################################################
#                                                                          #
#  file: CMakeLists.txt                                                    #
#  Copyright (C) 2016                                                      #
#      Enrico Bertolazzi                                                   #
#      Dipartimento di Ingegneria Industriale                              #
#      Universita` degli Studi di Trento                                   #
#      email: enrico.bertolazzi@unitn.it                                   #
#                                                                          #
############################################################################

cmake_minimum_required(VERSION 3.14)

project( GenericContainer )
include(./cmake_utils/CMakeLists-common.txt)

project(
  ${PROJECT_NAME}
  VERSION ${UTILS_PROJECT_VERSION}
  HOMEPAGE_URL "https://ebertolazzi.github.io/GenericContainer/"
)

include(./cmake_utils/CMakeLists-cflags.txt)
include(./cmake_utils/CMakeLists-utilities.txt)

# Evaluating the suffix to append to target built by this CMake file
utils_artifacts_suffix(ARTIFACTS_STATIC_SUFFIX TRUE)
if( UTILS_BUILD_SHARED )
  utils_artifacts_suffix(ARTIFACTS_DYNAMIC_SUFFIX FALSE)
endif()

message( STATUS "Compiler used: ${CMAKE_CXX_COMPILER_ID}" )
message( STATUS "BASE SUFFIXES = ${ARTIFACTS_STATIC_SUFFIX} ${ARTIFACTS_DYNAMIC_SUFFIX}" )

if( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../CMakeLists-customize.txt" )
  include(../CMakeLists-customize.txt)
endif()

if (APPLE)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLUA_USE_MACOSX")
elseif(LINUX OR UNIX)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLUA_USE_LINUX")
else()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
endif()

set(LUALIB Lua)

#   ___
#  / __| ___ _  _ _ _ __ ___ ___
#  \__ \/ _ \ || | '_/ _/ -_|_-<
#  |___/\___/\_,_|_| \__\___/__/
#
set( SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src" )

set( SOURCES )
file(
  GLOB S
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/src_json_interface/*.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/src_lua_interface/*.cc
)
foreach(F ${S})
  # build relative path respect to source directory
  file( RELATIVE_PATH RF ${CMAKE_CURRENT_SOURCE_DIR} "${F}" )
  list( APPEND SOURCES ${RF} )
endforeach(F ${S})

#list(FILTER SOURCES EXCLUDE REGEX ".*hsl_fake.cc$")

#   _  _             _
#  | || |___ __ _ __| |___ _ _ ___
#  | __ / -_) _` / _` / -_) '_(_-<
#  |_||_\___\__,_\__,_\___|_| /__/
#
set( PUBLIC_HEADERS )
file(
  GLOB S
  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h*
  ${CMAKE_CURRENT_SOURCE_DIR}/include/*/*.h*
  ${CMAKE_CURRENT_SOURCE_DIR}/include/*/*/*.h*
  ${CMAKE_CURRENT_SOURCE_DIR}/include/*/*/*/*.h*
)
foreach (F ${S})
  # build relative path respect to source directory
  file( RELATIVE_PATH RF "${CMAKE_CURRENT_SOURCE_DIR}/include" "${F}" )
  list( APPEND PUBLIC_HEADERS ${RF} )
endforeach(F ${S})

# extra include directory for compilation
include_directories( ./include ./src_json_interface )

#  _    _   _   _
# | |  | | | | / \
# | |  | | | |/ _ \
# | |__| |_| / ___ \
# |_____\___/_/   \_\
#
# LUA LIBRARY
set( lualib_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src_lua_interface/lua-5.4.4/src/" )

set( lualib_HEADERS )
file( GLOB S "${lualib_DIR}/lua*.h*" "${lualib_DIR}/lauxlib.h" )
foreach (F ${S})
  # build relative path respect to source directory
  file( RELATIVE_PATH RF "${lualib_DIR}" "${F}" )
  list( APPEND lualib_HEADERS ${RF} )
endforeach(F ${S})

set( lualib_SOURCES )
file( GLOB S "${lualib_DIR}/*.c" )
foreach (F ${S})
  # build relative path respect to source directory
  file( RELATIVE_PATH RF "${lualib_DIR}" "${F}" )
  list( APPEND lualib_SOURCES ${RF} )
endforeach(F ${S})

list( FILTER lualib_SOURCES EXCLUDE REGEX ".*luac?\.c$" )

#   _____                  _
#  |_   _|_ _ _ _ __ _ ___| |_ ___
#    | |/ _` | '_/ _` / -_)  _(_-<
#    |_|\__,_|_| \__, \___|\__/__/
#                |___/
#
# define target
utils_setup_target(
  Lua
  TRUE # build static
  "${lualib_DIR}"
  "${lualib_SOURCES}"
  "${lualib_DIR}"
  "${lualib_HEADERS}"
)
utils_setup_target(
  ${PROJECT_NAME}
  TRUE # build static
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${SOURCES}"
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${PUBLIC_HEADERS}"
)
if ( UTILS_BUILD_SHARED )
  utils_setup_target(
    Lua
    FALSE # build dynamic
    "${lualib_DIR}"
    "${lualib_SOURCES}"
    "${lualib_DIR}"
    "${lualib_HEADERS}"
  )
  utils_setup_target(
    ${PROJECT_NAME}
    FALSE # build dynamic
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${SOURCES}"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${PUBLIC_HEADERS}"
  )
  target_link_libraries( "${UTILS_NAMESPACE}_${PROJECT_NAME}" "${UTILS_NAMESPACE}_Lua" )
endif()

#   _____       _
#  |_   _|__ __| |_ ___
#    | |/ -_|_-<  _(_-<
#    |_|\___/__/\__/__/
#
if ( UTILS_ENABLE_TESTS )

  enable_testing()

  set( EXELISTBIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin" )
  set( EXELISTCPP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src_tests" )

  set(
    EXELISTCPP
    example1 example2 example3 example4 example5
    example7 example8 example9 example10
  )

  set( EXELISTC example6 )
  set( EXEJSONTEST JSON_test_1 )
  set( EXELUATEST example_lua1 example_lua2 example_lua3 )

  add_custom_target( "${PROJECT_NAME}_all_tests" ALL )

  set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin )
  set( LIBS ${UTILS_NAMESPACE}_${PROJECT_NAME}_Static ${UTILS_NAMESPACE}_Lua_Static ${CMAKE_DL_LIBS} )

  foreach( S ${EXELISTCPP} )
    add_executable( ${S} ${CMAKE_CURRENT_SOURCE_DIR}/examples/${S}.cc )
    target_link_libraries( ${S} ${LIBS} )
    set_target_properties( ${S} PROPERTIES SUFFIX ".exe" )
    add_test( NAME "${S}" COMMAND ./bin/${S}.exe WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
    add_dependencies( "${PROJECT_NAME}_all_tests" ${S} )
  endforeach()
  foreach( S ${EXELISTC} )
    add_executable( ${S} ${CMAKE_CURRENT_SOURCE_DIR}/examples/${S}.c )
    target_link_libraries( ${S} ${LIBS} )
    set_target_properties( ${S} PROPERTIES SUFFIX ".exe" )
    add_test( NAME "${S}" COMMAND ./bin/${S}.exe WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
    add_dependencies( "${PROJECT_NAME}_all_tests" ${S} )
  endforeach()
  foreach( S ${EXEJSONTEST} )
    add_executable( ${S} ${CMAKE_CURRENT_SOURCE_DIR}/src_json_interface/tests/${S}.cc )
    target_link_libraries( ${S} ${LIBS} )
    set_target_properties( ${S} PROPERTIES SUFFIX ".exe" )
    add_test( NAME "${S}" COMMAND ./bin/${S}.exe WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
    add_dependencies( "${PROJECT_NAME}_all_tests" ${S} )
  endforeach()
  foreach( S ${EXELUATEST} )
    add_executable( ${S} ${CMAKE_CURRENT_SOURCE_DIR}/src_lua_interface/tests/${S}.cc )
    target_link_libraries( ${S} ${LIBS} )
    set_target_properties( ${S} PROPERTIES SUFFIX ".exe" )
    add_test( NAME "${S}" COMMAND ./bin/${S}.exe WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
    add_dependencies( "${PROJECT_NAME}_all_tests" ${S} )
  endforeach()

endif()

#   ___         _        _ _
#  |_ _|_ _  __| |_ __ _| | |
#   | || ' \(_-<  _/ _` | | |
#  |___|_||_/__/\__\__,_|_|_|
#
install(
  TARGETS ${TARGETS_TO_BE_INSTALLED}
  # EXPORT (da definire)
  LIBRARY DESTINATION ${INSTALL_PREFIX}/dll
  ARCHIVE DESTINATION ${INSTALL_PREFIX}/lib
  RUNTIME DESTINATION ${INSTALL_PREFIX}/bin
  PUBLIC_HEADER DESTINATION ${PROJECT_PUBLIC_HEADER_DESTINATION}
)

#   ___         _
#  | _ \__ _ __| |____ _ __ _ ___
#  |  _/ _` / _| / / _` / _` / -_)
#  |_| \__,_\__|_\_\__,_\__, \___|
#                       |___/
#

# do not work
#include( ./cmake_utils/CMakeLists-cpack.txt )

utils_final_messages()
