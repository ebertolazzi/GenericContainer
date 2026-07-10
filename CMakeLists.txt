############################################################################
#                                                                          #
#  GenericContainer -- modern, idiomatic CMake build.                     #
#                                                                          #
#  Covers the core library (src/, include/), its Catch2 test battery      #
#  (tests/), and the optional json/yaml/toml/lua bridges. JSON/YAML/TOML  #
#  are compiled directly into GenericContainer; Lua stays as its own       #
#  runtime library, linked by GenericContainer when the Lua bridge is      #
#  enabled. The real library names stay standard (libGenericContainer.*    #
#  and libLua.*), and extra alias files such as libGenericContainer_osx.*  #
#  and libLua_osx.* are generated alongside them.                          #
#                                                                          #
############################################################################

cmake_minimum_required( VERSION 3.25 )

# Estrai la versione da Git tag (se disponibile)
find_package(Git QUIET)
if(Git_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=0
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )
  if(GIT_VERSION MATCHES "^v")
    string(SUBSTRING ${GIT_VERSION} 1 -1 GIT_VERSION)
  endif()
endif()
if(NOT GIT_VERSION)
  set(GIT_VERSION "0.0.0")
endif()


project(
  GenericContainer
  VERSION ${GIT_VERSION}
  DESCRIPTION "A C++ class to store heterogeneous, possibly recursive data"
  HOMEPAGE_URL "https://github.com/pbosetti/GenericContainer"
  LANGUAGES CXX
)

# Default to a Release build when the generator uses a single configuration
# and the user hasn't picked one (multi-config generators like Ninja
# Multi-Config or Visual Studio ignore CMAKE_BUILD_TYPE entirely).
if ( NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES )
  set( CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE )
  set_property( CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel" )
endif()

# Standalone vs. consumed-as-a-dependency: tests only make sense standalone,
# and installing shouldn't be forced on a project that FetchContent's us.
option( BUILD_SHARED_LIBS "Build GenericContainer as a shared library" OFF )
option( GENERIC_CONTAINER_INSTALL "Generate the install target" ${PROJECT_IS_TOP_LEVEL} )

# Each format bridge is opt-in; turn one off to skip fetching/building its
# third-party dependency entirely.
option( GENERIC_CONTAINER_ENABLE_JSON "Build GenericContainer::Json (nlohmann::json-backed)" ON )
option( GENERIC_CONTAINER_ENABLE_YAML "Build GenericContainer::Yaml (fkYAML-backed)" ON )
option( GENERIC_CONTAINER_ENABLE_TOML "Build GenericContainer::Toml (toml++-backed)" ON )
option( GENERIC_CONTAINER_ENABLE_LUA  "Build GenericContainer::Lua (embeds Lua 5.4)"  ON )
option( GENERIC_CONTAINER_BUILD_EXAMPLES "Build runnable executables from examples/" ${PROJECT_IS_TOP_LEVEL} )

# The backend subdirectories use FetchContent for their own dependencies.
# These cache paths let a developer keep sibling checkouts next to this
# project and avoid downloading them again. Set any path to a non-existing
# directory to force the corresponding FetchContent fallback.
set(
  GENERIC_CONTAINER_FKYAML_LOCAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../fkYAML"
  CACHE PATH "Path to a local fkYAML checkout to use before FetchContent"
)
set(
  GENERIC_CONTAINER_TOMLPLUSPLUS_LOCAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../tomlplusplus"
  CACHE PATH "Path to a local toml++ checkout to use before FetchContent"
)
set(
  GENERIC_CONTAINER_LUA_LOCAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../lua"
  CACHE PATH "Path to a local Lua checkout to use before FetchContent"
)

include( GNUInstallDirs )
include( FetchContent )

if ( APPLE )
  set( GENERIC_CONTAINER_PLATFORM_SUFFIX osx )
elseif ( WIN32 )
  if ( MINGW )
    if ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
      set( GENERIC_CONTAINER_PLATFORM_SUFFIX mingw64 )
    else()
      set( GENERIC_CONTAINER_PLATFORM_SUFFIX mingw32 )
    endif()
  elseif ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set( GENERIC_CONTAINER_PLATFORM_SUFFIX win64 )
  else()
    set( GENERIC_CONTAINER_PLATFORM_SUFFIX win32 )
  endif()
elseif ( UNIX )
  set( GENERIC_CONTAINER_PLATFORM_SUFFIX linux )
else()
  string( TOLOWER "${CMAKE_SYSTEM_NAME}" GENERIC_CONTAINER_PLATFORM_SUFFIX )
endif()

# Collected during configuration and printed once near the end.
set( GENERIC_CONTAINER_NLOHMANN_JSON_SOURCE "disabled" )
set( GENERIC_CONTAINER_FKYAML_SOURCE "disabled" )
set( GENERIC_CONTAINER_TOMLPLUSPLUS_SOURCE "disabled" )
set( GENERIC_CONTAINER_LUA_SOURCE "disabled" )

#   _     _ _
#  | |   (_) |__  _ __ __ _ _ __ _   _
#  | |   | | '_ \| '__/ _` | '__| | | |
#  | |___| | |_) | | | (_| | |  | |_| |
#  |_____|_|_.__/|_|  \__,_|_|   \__, |
#                                |___/
#
add_library(
  GenericContainer
  src/GenericContainer.cc
  src/GenericContainerCollapse.cc
  src/GenericContainerInterface_C.cc
  src/GenericContainerPromote.cc
  src/GenericContainerSerialize.cc
  src/GenericContainerSupport.cc
  src/GenericContainerTables.cc
  src/GenericContainer_ffi.cc
)
add_library( GenericContainer::GenericContainer ALIAS GenericContainer )
set( GENERIC_CONTAINER_EXPORTED_TARGETS GenericContainer )

target_compile_features( GenericContainer PUBLIC cxx_std_20 )
target_include_directories(
  GenericContainer PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

set_target_properties(
  GenericContainer PROPERTIES
  VERSION   ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  EXPORT_NAME GenericContainer
)

if ( CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU" )
  target_compile_options( GenericContainer PRIVATE -Wall -Wextra )
endif()

#   ____             _                  _
#  | __ )  __ _  ___| | _____ _ __   __| |___
#  |  _ \ / _` |/ __| |/ / _ \ '_ \ / _` / __|
#  | |_) | (_| | (__|   <  __/ | | | (_| \__ \
#  |____/ \__,_|\___|_|\_\___|_| |_|\__,_|___/
#
# Optional format bridges, each fetching its own dependency. JSON/YAML/TOML
# compile directly into GenericContainer. Lua keeps its own runtime library,
# exported as GenericContainer::LuaVM, while GenericContainer::Lua remains a
# compatibility interface target that links the full bridge stack.
#
# FetchContent has a standard escape hatch: if
# FETCHCONTENT_SOURCE_DIR_<NAME> is set, MakeAvailable(<name>) uses that
# source tree instead of downloading/populating it. Set those variables here
# before entering the backend subdirectories, where fkYAML/toml++/Lua are
# declared. The names below intentionally match the expected FetchContent
# dependency names used by the backend CMake files.
function( generic_container_prefer_local_fetchcontent DEPENDENCY_NAME LOCAL_DIR SUMMARY_VARIABLE )
  string( TOUPPER "${DEPENDENCY_NAME}" DEPENDENCY_NAME_UPPER )
  set( SOURCE_DIR_VARIABLE "FETCHCONTENT_SOURCE_DIR_${DEPENDENCY_NAME_UPPER}" )
  set( ${SUMMARY_VARIABLE} "FetchContent" PARENT_SCOPE )

  if ( DEFINED ${SOURCE_DIR_VARIABLE} AND IS_DIRECTORY "${${SOURCE_DIR_VARIABLE}}" )
    message( STATUS "Using local ${DEPENDENCY_NAME} checkout: ${${SOURCE_DIR_VARIABLE}}" )
    set( ${SUMMARY_VARIABLE} "local: ${${SOURCE_DIR_VARIABLE}}" PARENT_SCOPE )
    return()
  endif()

  get_filename_component(
    LOCAL_DIR_ABS
    "${LOCAL_DIR}"
    ABSOLUTE
    BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
  )

  if ( IS_DIRECTORY "${LOCAL_DIR_ABS}" )
    message( STATUS "Using local ${DEPENDENCY_NAME} checkout: ${LOCAL_DIR_ABS}" )
    set(
      ${SOURCE_DIR_VARIABLE} "${LOCAL_DIR_ABS}"
      CACHE PATH "Local source directory for ${DEPENDENCY_NAME}; overrides FetchContent download"
      FORCE
    )
    set( ${SUMMARY_VARIABLE} "local: ${LOCAL_DIR_ABS}" PARENT_SCOPE )
  else()
    set( ${SUMMARY_VARIABLE} "FetchContent" PARENT_SCOPE )
  endif()
endfunction()

function( generic_container_append_exported_target TARGET_NAME )
  set( updated_exported_targets ${GENERIC_CONTAINER_EXPORTED_TARGETS} )
  list( APPEND updated_exported_targets ${TARGET_NAME} )
  set( GENERIC_CONTAINER_EXPORTED_TARGETS "${updated_exported_targets}" PARENT_SCOPE )
endfunction()

function( generic_container_apply_standard_library_name TARGET_NAME BASE_NAME )
  if ( WIN32 )
    set( TARGET_OUTPUT_NAME "lib${BASE_NAME}" )
    set_target_properties( ${TARGET_NAME} PROPERTIES PREFIX "" IMPORT_PREFIX "" )
  else()
    set( TARGET_OUTPUT_NAME "${BASE_NAME}" )
  endif()

  set_target_properties(
    ${TARGET_NAME} PROPERTIES
    OUTPUT_NAME         "${TARGET_OUTPUT_NAME}"
    ARCHIVE_OUTPUT_NAME "${TARGET_OUTPUT_NAME}"
    LIBRARY_OUTPUT_NAME "${TARGET_OUTPUT_NAME}"
    RUNTIME_OUTPUT_NAME "${TARGET_OUTPUT_NAME}"
  )
endfunction()

function( generic_container_add_platform_library_alias TARGET_NAME BASE_NAME )
  set( TARGET_ALIAS_BASE "lib${BASE_NAME}_${GENERIC_CONTAINER_PLATFORM_SUFFIX}" )
  set( LEGACY_TARGET_ALIAS_BASE "${TARGET_ALIAS_BASE}" )
  if ( NOT BUILD_SHARED_LIBS )
    string( APPEND TARGET_ALIAS_BASE "_static" )
  endif()

  if ( BUILD_SHARED_LIBS AND WIN32 )
    set( TARGET_RUNTIME_FILENAME "lib${BASE_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}" )
    set( TARGET_IMPORT_FILENAME  "lib${BASE_NAME}${CMAKE_IMPORT_LIBRARY_SUFFIX}" )
    set( TARGET_ALIAS_RUNTIME    "${TARGET_ALIAS_BASE}${CMAKE_SHARED_LIBRARY_SUFFIX}" )
    set( TARGET_ALIAS_IMPORT     "${TARGET_ALIAS_BASE}${CMAKE_IMPORT_LIBRARY_SUFFIX}" )

    add_custom_command(
      TARGET ${TARGET_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:${TARGET_NAME}>"
        "$<TARGET_FILE_DIR:${TARGET_NAME}>/${TARGET_ALIAS_RUNTIME}"
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_LINKER_FILE:${TARGET_NAME}>"
        "$<TARGET_LINKER_FILE_DIR:${TARGET_NAME}>/${TARGET_ALIAS_IMPORT}"
      VERBATIM
    )

    install(
      FILES "$<TARGET_FILE_DIR:${TARGET_NAME}>/${TARGET_ALIAS_RUNTIME}"
      DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    install(
      FILES "$<TARGET_LINKER_FILE_DIR:${TARGET_NAME}>/${TARGET_ALIAS_IMPORT}"
      DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
  elseif ( BUILD_SHARED_LIBS )
    set( TARGET_LINK_FILENAME "${CMAKE_SHARED_LIBRARY_PREFIX}${BASE_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}" )
    set( TARGET_ALIAS_FILENAME "${TARGET_ALIAS_BASE}${CMAKE_SHARED_LIBRARY_SUFFIX}" )

    add_custom_command(
      TARGET ${TARGET_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E rm -f
        "$<TARGET_LINKER_FILE_DIR:${TARGET_NAME}>/${TARGET_ALIAS_FILENAME}"
      COMMAND ${CMAKE_COMMAND} -E create_symlink
        "${TARGET_LINK_FILENAME}"
        "$<TARGET_LINKER_FILE_DIR:${TARGET_NAME}>/${TARGET_ALIAS_FILENAME}"
      VERBATIM
    )

    install(
      CODE
      "file(
        MAKE_DIRECTORY
        \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}\"
      )
      execute_process(
        COMMAND \"${CMAKE_COMMAND}\" -E rm -f
          \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${TARGET_ALIAS_FILENAME}\"
      )
      execute_process(
        COMMAND \"${CMAKE_COMMAND}\" -E create_symlink
          \"${TARGET_LINK_FILENAME}\"
          \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${TARGET_ALIAS_FILENAME}\"
      )"
    )
  else()
    if ( WIN32 )
      set( TARGET_LINK_FILENAME "lib${BASE_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}" )
      set( TARGET_ALIAS_FILENAME "${TARGET_ALIAS_BASE}${CMAKE_STATIC_LIBRARY_SUFFIX}" )
      set( LEGACY_TARGET_ALIAS_FILENAME "${LEGACY_TARGET_ALIAS_BASE}${CMAKE_STATIC_LIBRARY_SUFFIX}" )
    else()
      set( TARGET_LINK_FILENAME "${CMAKE_STATIC_LIBRARY_PREFIX}${BASE_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}" )
      set( TARGET_ALIAS_FILENAME "${TARGET_ALIAS_BASE}${CMAKE_STATIC_LIBRARY_SUFFIX}" )
      set( LEGACY_TARGET_ALIAS_FILENAME "${LEGACY_TARGET_ALIAS_BASE}${CMAKE_STATIC_LIBRARY_SUFFIX}" )
    endif()

    if ( WIN32 )
      add_custom_command(
        TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E rm -f
          "$<TARGET_FILE_DIR:${TARGET_NAME}>/${LEGACY_TARGET_ALIAS_FILENAME}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "$<TARGET_FILE:${TARGET_NAME}>"
          "$<TARGET_FILE_DIR:${TARGET_NAME}>/${TARGET_ALIAS_FILENAME}"
        VERBATIM
      )

      install(
        CODE
        "execute_process(
          COMMAND \"${CMAKE_COMMAND}\" -E rm -f
            \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${LEGACY_TARGET_ALIAS_FILENAME}\"
        )"
      )
      install(
        FILES "$<TARGET_FILE_DIR:${TARGET_NAME}>/${TARGET_ALIAS_FILENAME}"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
      )
      return()
    endif()

    add_custom_command(
      TARGET ${TARGET_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E rm -f
        "$<TARGET_FILE_DIR:${TARGET_NAME}>/${LEGACY_TARGET_ALIAS_FILENAME}"
      COMMAND ${CMAKE_COMMAND} -E rm -f
        "$<TARGET_FILE_DIR:${TARGET_NAME}>/${TARGET_ALIAS_FILENAME}"
      COMMAND ${CMAKE_COMMAND} -E create_symlink
        "${TARGET_LINK_FILENAME}"
        "$<TARGET_FILE_DIR:${TARGET_NAME}>/${TARGET_ALIAS_FILENAME}"
      VERBATIM
    )

    install(
      CODE
      "file(
        MAKE_DIRECTORY
        \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}\"
      )
      execute_process(
        COMMAND \"${CMAKE_COMMAND}\" -E rm -f
          \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${LEGACY_TARGET_ALIAS_FILENAME}\"
      )
      execute_process(
        COMMAND \"${CMAKE_COMMAND}\" -E rm -f
          \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${TARGET_ALIAS_FILENAME}\"
      )
      execute_process(
        COMMAND \"${CMAKE_COMMAND}\" -E create_symlink
          \"${TARGET_LINK_FILENAME}\"
          \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${TARGET_ALIAS_FILENAME}\"
      )"
    )
  endif()
endfunction()

function( generic_container_add_backend_alias TARGET_NAME EXPORT_NAME )
  add_library( ${TARGET_NAME} INTERFACE )
  add_library( GenericContainer::${EXPORT_NAME} ALIAS ${TARGET_NAME} )
  set_target_properties( ${TARGET_NAME} PROPERTIES EXPORT_NAME ${EXPORT_NAME} )
  target_link_libraries( ${TARGET_NAME} INTERFACE GenericContainer::GenericContainer )

  set( updated_exported_targets ${GENERIC_CONTAINER_EXPORTED_TARGETS} )
  list( APPEND updated_exported_targets ${TARGET_NAME} )
  set( GENERIC_CONTAINER_EXPORTED_TARGETS "${updated_exported_targets}" PARENT_SCOPE )
endfunction()

function( generic_container_ensure_nlohmann_json )
  if ( TARGET nlohmann_json::nlohmann_json )
    set( GENERIC_CONTAINER_NLOHMANN_JSON_SOURCE "existing target: nlohmann_json::nlohmann_json" PARENT_SCOPE )
    return()
  endif()

  find_package( nlohmann_json 3 QUIET )
  if ( TARGET nlohmann_json::nlohmann_json )
    set( GENERIC_CONTAINER_NLOHMANN_JSON_SOURCE "find_package" PARENT_SCOPE )
    return()
  endif()

  FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/include.zip
  )
  FetchContent_MakeAvailable( nlohmann_json )
  add_library( nlohmann_json INTERFACE )
  add_library( nlohmann_json::nlohmann_json ALIAS nlohmann_json )
  target_include_directories(
    nlohmann_json SYSTEM INTERFACE
    "$<BUILD_INTERFACE:${nlohmann_json_SOURCE_DIR}/include>"
  )
  set( GENERIC_CONTAINER_NLOHMANN_JSON_SOURCE "FetchContent: v3.11.3" PARENT_SCOPE )
endfunction()

if ( GENERIC_CONTAINER_ENABLE_JSON )
  generic_container_ensure_nlohmann_json()
endif()
if ( GENERIC_CONTAINER_ENABLE_YAML )
  generic_container_prefer_local_fetchcontent( fkYAML "${GENERIC_CONTAINER_FKYAML_LOCAL_DIR}" GENERIC_CONTAINER_FKYAML_SOURCE )
endif()
if ( GENERIC_CONTAINER_ENABLE_TOML )
  generic_container_prefer_local_fetchcontent( tomlplusplus "${GENERIC_CONTAINER_TOMLPLUSPLUS_LOCAL_DIR}" GENERIC_CONTAINER_TOMLPLUSPLUS_SOURCE )
endif()
if ( GENERIC_CONTAINER_ENABLE_LUA )
  generic_container_prefer_local_fetchcontent( lua54 "${GENERIC_CONTAINER_LUA_LOCAL_DIR}" GENERIC_CONTAINER_LUA_SOURCE )
endif()

generic_container_apply_standard_library_name( GenericContainer GenericContainer )
generic_container_add_platform_library_alias( GenericContainer GenericContainer )

if ( GENERIC_CONTAINER_ENABLE_JSON )
  target_sources( GenericContainer PRIVATE interfaces/json/GenericContainerJson.cc )
  target_link_libraries( GenericContainer PRIVATE $<BUILD_INTERFACE:nlohmann_json::nlohmann_json> )
  generic_container_add_backend_alias( GenericContainerJson Json )
endif()
if ( GENERIC_CONTAINER_ENABLE_YAML )
  find_package( fkYAML 0.4 QUIET )
  if ( NOT TARGET fkYAML::fkYAML )
    FetchContent_Declare(
      fkYAML
      GIT_REPOSITORY https://github.com/fktn-k/fkYAML.git
      GIT_TAG        v0.4.2
    )
    FetchContent_MakeAvailable( fkYAML )
  endif()

  target_sources(
    GenericContainer PRIVATE
    interfaces/yaml/from_yaml.cc
    interfaces/yaml/to_yaml.cc
  )
  target_link_libraries( GenericContainer PRIVATE $<BUILD_INTERFACE:fkYAML::fkYAML> )
  generic_container_add_backend_alias( GenericContainerYaml Yaml )
endif()
if ( GENERIC_CONTAINER_ENABLE_TOML )
  find_package( tomlplusplus 3.4 QUIET )
  if ( NOT TARGET tomlplusplus::tomlplusplus )
    FetchContent_Declare(
      tomlplusplus
      GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
      GIT_TAG        v3.4.0
    )
    FetchContent_MakeAvailable( tomlplusplus )
  endif()

  target_sources(
    GenericContainer PRIVATE
    interfaces/toml/from_toml.cc
    interfaces/toml/to_toml.cc
  )
  target_link_libraries( GenericContainer PRIVATE $<BUILD_INTERFACE:tomlplusplus::tomlplusplus> )
  generic_container_add_backend_alias( GenericContainerToml Toml )
endif()
if ( GENERIC_CONTAINER_ENABLE_LUA )
  enable_language( C )

  FetchContent_Declare(
    lua54
    URL https://www.lua.org/ftp/lua-5.4.6.tar.gz
    SOURCE_SUBDIR doc
  )
  FetchContent_MakeAvailable( lua54 )

  if ( EXISTS "${lua54_SOURCE_DIR}/src/lua.h" )
    set( GENERIC_CONTAINER_LUA_SOURCE_DIR "${lua54_SOURCE_DIR}/src" )
  else()
    set( GENERIC_CONTAINER_LUA_SOURCE_DIR "${lua54_SOURCE_DIR}" )
  endif()

  file( GLOB LUA_SOURCES CONFIGURE_DEPENDS "${GENERIC_CONTAINER_LUA_SOURCE_DIR}/*.c" )
  list( FILTER LUA_SOURCES EXCLUDE REGEX "/lua\\.c$" )
  list( FILTER LUA_SOURCES EXCLUDE REGEX "/luac\\.c$" )
  list( FILTER LUA_SOURCES EXCLUDE REGEX "/onelua\\.c$" )

  add_library( Lua ${LUA_SOURCES} )
  add_library( GenericContainer::LuaVM ALIAS Lua )
  set_target_properties( Lua PROPERTIES EXPORT_NAME LuaVM )
  target_include_directories(
    Lua PUBLIC
    "$<BUILD_INTERFACE:${GENERIC_CONTAINER_LUA_SOURCE_DIR}>"
  )

  if ( APPLE )
    target_compile_definitions( Lua PRIVATE LUA_USE_MACOSX )
  elseif ( UNIX )
    target_compile_definitions( Lua PRIVATE LUA_USE_LINUX )
    target_link_libraries( Lua PUBLIC m ${CMAKE_DL_LIBS} )
  endif()

  generic_container_apply_standard_library_name( Lua Lua )
  generic_container_add_platform_library_alias( Lua Lua )
  generic_container_append_exported_target( Lua )

  target_sources(
    GenericContainer PRIVATE
    interfaces/lua/GenericContainerInterface_lua.cc
    interfaces/lua/GenericContainerLuaPmain.cc
  )
  target_link_libraries( GenericContainer PUBLIC Lua )

  generic_container_add_backend_alias( GenericContainerLua Lua )
endif()

#   _____         _
#  |_   _|__  ___| |_ ___
#    | |/ _ \/ __| __/ __|
#    | |  __/\__ \ |_\__ \
#    |_|\___||___/\__|___/
#
include( CTest )
if ( BUILD_TESTING AND PROJECT_IS_TOP_LEVEL )
  find_package( Catch2 3 QUIET )
  if ( NOT TARGET Catch2::Catch2WithMain )
    FetchContent_Declare(
      Catch2
      GIT_REPOSITORY https://github.com/catchorg/Catch2.git
      GIT_TAG        v3.7.1
    )
    FetchContent_MakeAvailable( Catch2 )

    foreach( catch2_target Catch2 Catch2WithMain )
      if ( TARGET ${catch2_target} )
        target_compile_options( ${catch2_target} PRIVATE $<IF:$<CXX_COMPILER_ID:MSVC>,/w,-w> )
        target_compile_features( ${catch2_target} PUBLIC cxx_std_20 )
      endif()
    endforeach()

    list( APPEND CMAKE_MODULE_PATH "${Catch2_SOURCE_DIR}/extras" )
  endif()

  generic_container_ensure_nlohmann_json()

  file( GLOB GC_TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_*.cc" )
  list(
    REMOVE_ITEM GC_TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_ffi.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_tables.cc"
  )

  add_executable( gc_unit_tests ${GC_TEST_SOURCES} )
  target_link_libraries(
    gc_unit_tests PRIVATE
    GenericContainer::GenericContainer
    nlohmann_json::nlohmann_json
    Catch2::Catch2WithMain
  )

  include( Catch )
  catch_discover_tests(
    gc_unit_tests
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    PROPERTIES LABELS "unit"
  )
endif()

#   _____                           _
#  | ____|_  ____ _ _ __ ___  _ __ | | ___  ___
#  |  _| \ \/ / _` | '_ ` _ \| '_ \| |/ _ \/ __|
#  | |___ >  < (_| | | | | | | |_) | |  __/\__ \
#  |_____/_/\_\__,_|_| |_| |_| .__/|_|\___||___/
#                            |_|
if ( GENERIC_CONTAINER_BUILD_EXAMPLES AND PROJECT_IS_TOP_LEVEL )
  set( GENERIC_CONTAINER_EXAMPLE_BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin" )

  function( generic_container_add_example EXAMPLE_NAME )
    set( options )
    set( one_value_args )
    set( multi_value_args LINK_LIBS )
    cmake_parse_arguments( ARG "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN} )

    add_executable( ${EXAMPLE_NAME} "examples/${EXAMPLE_NAME}.cc" )
    target_compile_features( ${EXAMPLE_NAME} PRIVATE cxx_std_20 )
    if ( ARG_LINK_LIBS )
      target_link_libraries( ${EXAMPLE_NAME} PRIVATE ${ARG_LINK_LIBS} )
    else()
      target_link_libraries( ${EXAMPLE_NAME} PRIVATE GenericContainer::GenericContainer )
    endif()
    set_target_properties( ${EXAMPLE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${GENERIC_CONTAINER_EXAMPLE_BIN_DIR}" )
    foreach( cfg Debug Release RelWithDebInfo MinSizeRel )
      string( TOUPPER "${cfg}" cfg_upper )
      set_target_properties( ${EXAMPLE_NAME} PROPERTIES "RUNTIME_OUTPUT_DIRECTORY_${cfg_upper}" "${GENERIC_CONTAINER_EXAMPLE_BIN_DIR}" )
    endforeach()

    if ( BUILD_TESTING )
      add_test( NAME "example.${EXAMPLE_NAME}" COMMAND "${GENERIC_CONTAINER_EXAMPLE_BIN_DIR}/${EXAMPLE_NAME}" )
      set_tests_properties(
        "example.${EXAMPLE_NAME}" PROPERTIES
        LABELS "examples"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
      )
    endif()
  endfunction()

  foreach( example_name example01 example02 example03 example04 example05 example07 example08 example09 example10 example17 )
    generic_container_add_example( ${example_name} )
  endforeach()

  if ( TARGET GenericContainer::Yaml )
    foreach( example_name example11 example12 )
      generic_container_add_example( ${example_name} LINK_LIBS GenericContainer::Yaml )
    endforeach()
  endif()

  if ( TARGET GenericContainer::Json )
    foreach( example_name example14 example15 )
      generic_container_add_example( ${example_name} LINK_LIBS GenericContainer::Json )
    endforeach()
  endif()

  if ( TARGET GenericContainer::Json AND TARGET GenericContainer::Yaml )
    foreach( example_name example13 example16 )
      generic_container_add_example( ${example_name} LINK_LIBS GenericContainer::Json GenericContainer::Yaml )
    endforeach()
  endif()

  if ( TARGET GenericContainer::Toml )
    generic_container_add_example( example18 LINK_LIBS GenericContainer::Toml )
  endif()
endif()

#   ___           _        _ _
#  |_ _|_ __  ___| |_ __ _| | |
#   | || '_ \/ __| __/ _` | | |
#   | || | | \__ \ || (_| | | |
#  |___|_| |_|___/\__\__,_|_|_|
#
if ( GENERIC_CONTAINER_INSTALL )
  include( CMakePackageConfigHelpers )

  set( GENERIC_CONTAINER_CONFIG_TEMPLATE "${CMAKE_CURRENT_BINARY_DIR}/GenericContainerConfig.cmake.in" )
  file(
    WRITE "${GENERIC_CONTAINER_CONFIG_TEMPLATE}"
    [=[
@PACKAGE_INIT@

include( "${CMAKE_CURRENT_LIST_DIR}/GenericContainerTargets.cmake" )

check_required_components( GenericContainer )
]=]
  )

  install(
    TARGETS ${GENERIC_CONTAINER_EXPORTED_TARGETS}
    EXPORT GenericContainerTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )
  install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PATTERN ".DS_Store" EXCLUDE
  )

  install(
    EXPORT GenericContainerTargets
    FILE GenericContainerTargets.cmake
    NAMESPACE GenericContainer::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GenericContainer
  )

  configure_package_config_file(
    "${GENERIC_CONTAINER_CONFIG_TEMPLATE}"
    "${CMAKE_CURRENT_BINARY_DIR}/GenericContainerConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GenericContainer
  )
  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/GenericContainerConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
  )
  install(
    FILES
    "${CMAKE_CURRENT_BINARY_DIR}/GenericContainerConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/GenericContainerConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GenericContainer
  )

  # `cpack` / the "package" target: source and binary archives.
  set( CPACK_PACKAGE_NAME "${PROJECT_NAME}" )
  set( CPACK_PACKAGE_VERSION "${PROJECT_VERSION}" )
  set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}" )
  set( CPACK_GENERATOR "TGZ;ZIP" )
  set( CPACK_SOURCE_GENERATOR "TGZ;ZIP" )
  set( CPACK_SOURCE_IGNORE_FILES "${CMAKE_CURRENT_BINARY_DIR}/;/\\\\.git/;/build.*/" )
  include( CPack )
endif()

#   ____                                   
#  / ___| _   _ _ __ ___  _ __ ___   __ _ _ __ _   _
#  \___ \| | | | '_ ` _ \| '_ ` _ \ / _` | '__| | | |
#   ___) | |_| | | | | | | | | | | | | (_| | |  | |_| |
#  |____/ \__,_|_| |_| |_|_| |_| |_|\__,_|_|   \__, |
#                                                  |___/ 
#
message( STATUS "" )
message( STATUS "GenericContainer dependency summary:" )
message( STATUS "  nlohmann_json: ${GENERIC_CONTAINER_NLOHMANN_JSON_SOURCE}" )
message( STATUS "  fkYAML       : ${GENERIC_CONTAINER_FKYAML_SOURCE}" )
message( STATUS "  tomlplusplus : ${GENERIC_CONTAINER_TOMLPLUSPLUS_SOURCE}" )
message( STATUS "  lua          : ${GENERIC_CONTAINER_LUA_SOURCE}" )
message( STATUS "" )
