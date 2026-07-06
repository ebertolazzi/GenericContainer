############################################################################
#                                                                          #
#  GenericContainer -- modern, idiomatic CMake build.                     #
#                                                                          #
#  Covers the core library (src/, include/), its Catch2 test battery      #
#  (tests/), and the yaml/toml/lua format-backend interfaces, each built  #
#  as a separate opt-in library (GenericContainer::Yaml/Toml/Lua) rather  #
#  than linked into the core, so the core stays self-contained and        #
#  standalone-linkable regardless of which backends a consumer needs.     #
#  JSON is covered by the header-only nlohmann::json adapter (see         #
#  include/GenericContainer/GenericContainerInterface_nlohmann.hh)        #
#  instead of a dedicated backend. src_matlab_interface/ and examples/    #
#  are not part of this build for now; they need their own                #
#  restructuring pass before rejoining it.                                #
#                                                                          #
############################################################################

cmake_minimum_required( VERSION 3.25 )

project(
  GenericContainer
  VERSION 1.1.0
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

# Each format backend is its own opt-in library; turn one off to skip
# fetching/building its (sole) third-party dependency entirely.
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
# Optional format-backend libraries, each fetching its own single
# dependency. toml++/fkYAML are header-only and never appear in either
# backend's public headers, so their effects are fully baked into
# GenericContainerToml/Yaml's compiled code once built ($<BUILD_INTERFACE:...>
# in their target_link_libraries keeps that dependency out of the installed
# package's metadata entirely -- see each subdirectory's CMakeLists.txt).
# Lua is different: it has real compiled code that must reach whatever
# finally links GenericContainer::Lua, so the "lua" target itself also
# joins the export set below (as GenericContainer::LuaVM).
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
  generic_container_prefer_local_fetchcontent( lua "${GENERIC_CONTAINER_LUA_LOCAL_DIR}" GENERIC_CONTAINER_LUA_SOURCE )
endif()

set( GENERIC_CONTAINER_BACKEND_TARGETS )
if ( GENERIC_CONTAINER_ENABLE_JSON )
  add_library( GenericContainerJson interfaces/json/GenericContainerJson.cc )
  add_library( GenericContainer::Json ALIAS GenericContainerJson )
  set_target_properties( GenericContainerJson PROPERTIES EXPORT_NAME Json )
  target_compile_features( GenericContainerJson PUBLIC cxx_std_20 )
  target_link_libraries(
    GenericContainerJson
    PUBLIC  GenericContainer::GenericContainer
    PRIVATE $<BUILD_INTERFACE:nlohmann_json::nlohmann_json>
  )
  list( APPEND GENERIC_CONTAINER_BACKEND_TARGETS GenericContainerJson )
endif()
if ( GENERIC_CONTAINER_ENABLE_YAML )
  add_subdirectory( interfaces/yaml )
  list( APPEND GENERIC_CONTAINER_BACKEND_TARGETS GenericContainerYaml )
endif()
if ( GENERIC_CONTAINER_ENABLE_TOML )
  add_subdirectory( interfaces/toml )
  list( APPEND GENERIC_CONTAINER_BACKEND_TARGETS GenericContainerToml )
endif()
if ( GENERIC_CONTAINER_ENABLE_LUA )
  add_subdirectory( interfaces/lua )
  list( APPEND GENERIC_CONTAINER_BACKEND_TARGETS GenericContainerLua lua )
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
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_support.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_json_yaml_toml.cc"
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
    TARGETS GenericContainer ${GENERIC_CONTAINER_BACKEND_TARGETS}
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
