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

cmake_minimum_required( VERSION 2.8.12 )
SET( CMAKE_VERBOSE_MAKEFILE ON )
SET( CMAKE_INSTALL_MESSAGE ALWAYS )

IF ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
  SET( DEBUG_SUFFIX "_debug" )
ELSE()
  SET( DEBUG_SUFFIX )
ENDIF()

IF ( BUILD_SHARED )
  IF ( APPLE )
    SET( TARGET GenericContainer_osx${DEBUG_SUFFIX} )
  ELSEIF( LINUX OR UNIX )
    SET( TARGET GenericContainer_linux${DEBUG_SUFFIX} )
  ELSE()
    SET( TARGET GenericContainer_win_${BITS}${DEBUG_SUFFIX} )
    SET( CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON )
  ENDIF()
ENDIF()

IF ( APPLE )
  SET( TARGETS GenericContainer_osx_static${DEBUG_SUFFIX} )
ELSEIF( LINUX OR UNIX )
  SET( TARGETS GenericContainer_linux_static${DEBUG_SUFFIX} )
ELSE()
  SET( TARGETS GenericContainer_win_${BITS}_static${DEBUG_SUFFIX} )
ENDIF()

PROJECT( GenericContainer CXX C )

INCLUDE( "CMakeLists-cflags.txt" )

MESSAGE( STATUS "Compiler used: ${CMAKE_CXX_COMPILER_ID}" )

IF ( APPLE )
  SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLUA_USE_MACOSX" )
ELSEIF( LINUX OR UNIX )
  SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLUA_USE_LINUX" )
ELSE()
  SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" )
ENDIF()

SET( SOURCES )
FILE( GLOB S  ./src/*.cc )
FILE( GLOB S1 ./src_json_interface/*.cc )
LIST( APPEND S ${S1} )
FILE( GLOB S1 ./src_lua_interface/*.cc )
LIST( APPEND S ${S1} )
FILE( GLOB S1 ./src_lua_interface/lua-5.4.3/src/*.c )
LIST( APPEND S ${S1} )
FOREACH (F ${S})
  FILE( RELATIVE_PATH RF ${CMAKE_CURRENT_SOURCE_DIR} "${F}" )
  LIST( APPEND SOURCES ${RF} )
ENDFOREACH (F ${S})

SET( HEADERS )
FILE( GLOB S ./src/*.hh)
FILE( GLOB S1 ./src/*.h)
LIST( APPEND S ${S1} )
FILE( GLOB S1 ./src_json_interface/*.hh)
LIST( APPEND S ${S1} )
FILE( GLOB S1 ./src_lua_interface/*.hh)
LIST( APPEND S ${S1} )
FOREACH (F ${S})
  FILE( RELATIVE_PATH RF ${CMAKE_CURRENT_SOURCE_DIR} "${F}" )
  LIST( APPEND HEADERS ${RF} )
ENDFOREACH (F ${S})

INCLUDE_DIRECTORIES( src src_json_interface src_lua_interface )

SET( CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/lib )

IF ( BUILD_SHARED )
  ADD_LIBRARY( ${TARGETS} STATIC ${SOURCES} ${HEADERS} )
  ADD_LIBRARY( ${TARGET}  SHARED ${SOURCES} ${HEADERS} )
  TARGET_LINK_LIBRARIES( ${TARGET} )
  INSTALL(
    TARGETS ${TARGET} ${TARGETS}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/dll
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
  )
ELSE()
  ADD_LIBRARY( ${TARGETS} STATIC ${SOURCES} ${HEADERS} )
  INSTALL(
    TARGETS ${TARGETS}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/dll
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
  )
ENDIF()

SET_PROPERTY( TARGET ${TARGETS} PROPERTY POSITION_INDEPENDENT_CODE ON )

INSTALL( FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include )

IF( BUILD_EXECUTABLE )

  SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

  ADD_EXECUTABLE( example1 examples/example1.cc )
  TARGET_LINK_LIBRARIES( example1 ${TARGETS} )

  ADD_EXECUTABLE( example2 examples/example2.cc )
  TARGET_LINK_LIBRARIES( example2 ${TARGETS} )

  ADD_EXECUTABLE( example3 examples/example3.cc )
  TARGET_LINK_LIBRARIES( example3 ${TARGETS} )

  ADD_EXECUTABLE( example4 examples/example4.cc )
  TARGET_LINK_LIBRARIES( example4 ${TARGETS} )

  ADD_EXECUTABLE( example5 examples/example5.cc )
  TARGET_LINK_LIBRARIES( example5 ${TARGETS} )

  ADD_EXECUTABLE( example6 examples/example6.c ) # IT'S C!
  TARGET_LINK_LIBRARIES( example6 ${TARGETS} )

  ADD_EXECUTABLE( example7 examples/example7.cc )
  TARGET_LINK_LIBRARIES( example7 ${TARGETS} )

  ADD_EXECUTABLE( example8 examples/example8.cc )
  TARGET_LINK_LIBRARIES( example8 ${TARGETS} )

  ADD_EXECUTABLE( example9 examples/example9.cc )
  TARGET_LINK_LIBRARIES( example9 ${TARGETS} )

  ADD_EXECUTABLE( JSON_test_1 src_json_interface/tests/JSON_test_1.cc )
  TARGET_LINK_LIBRARIES( JSON_test_1 ${TARGETS} )

  ADD_EXECUTABLE( example_lua1 src_lua_interface/tests/example_lua1.cc )
  TARGET_LINK_LIBRARIES( example_lua1 ${TARGETS} )

  ADD_EXECUTABLE( example_lua2 src_lua_interface/tests/example_lua2.cc )
  TARGET_LINK_LIBRARIES( example_lua2 ${TARGETS} )

  ADD_EXECUTABLE( example_lua3 src_lua_interface/tests/example_lua3.cc )
  TARGET_LINK_LIBRARIES( example_lua3 ${TARGETS} )

ENDIF()

MESSAGE( STATUS "C compiler                  = ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER}" )
MESSAGE( STATUS "C++ compiler                = ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER}" )
MESSAGE( STATUS "CMAKE_CXX_FLAGS_RELEASE     = ${CMAKE_CXX_FLAGS_RELEASE}" )
MESSAGE( STATUS "CMAKE_CXX_FLAGS             = ${CMAKE_CXX_FLAGS}" )
MESSAGE( STATUS "CMAKE_C_FLAGS_RELEASE       = ${CMAKE_C_FLAGS_RELEASE}" )
MESSAGE( STATUS "CMAKE_C_FLAGS               = ${CMAKE_C_FLAGS}" )
MESSAGE( STATUS "CMAKE_SYSTEM_NAME           = ${CMAKE_SYSTEM_NAME}" )
MESSAGE( STATUS "CMAKE_SYSTEM_PROCESSOR      = ${CMAKE_SYSTEM_PROCESSOR}" )
IF(APPLE)
  MESSAGE( STATUS "CMAKE_OSX_SYSROOT           = ${CMAKE_OSX_SYSROOT}" )
  MESSAGE( STATUS "CMAKE_OSX_ARCHITECTURES     = ${CMAKE_OSX_ARCHITECTURES}" )
  MESSAGE( STATUS "CMAKE_OSX_DEPLOYMENT_TARGET = ${CMAKE_OSX_DEPLOYMENT_TARGET}" )
ENDIF()
MESSAGE( STATUS "SOURCES = ${SOURCES}" )
MESSAGE( STATUS "HEADERS = ${HEADERS}" )
