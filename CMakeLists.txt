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
option( GENERIC_CONTAINER_ENABLE_YAML "Build GenericContainer::Yaml (fkYAML-backed)" ON )
option( GENERIC_CONTAINER_ENABLE_TOML "Build GenericContainer::Toml (toml++-backed)" ON )
option( GENERIC_CONTAINER_ENABLE_LUA  "Build GenericContainer::Lua (embeds Lua 5.4)"  ON )

# Eigen is header-only. By default, prefer a sibling Eigen checkout at
# ../eigen when present; otherwise fetch the pinned release below.
# Override with -DGENERIC_CONTAINER_EIGEN_LOCAL_DIR=/path/to/eigen, or set
# it to an invalid/empty path to force FetchContent.
set( GENERIC_CONTAINER_EIGEN_VERSION "5.0.0" CACHE STRING "Eigen version to fetch" )
set(
  GENERIC_CONTAINER_EIGEN_LOCAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../eigen"
  CACHE PATH "Path to a local Eigen checkout to use before FetchContent"
)

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
set( GENERIC_CONTAINER_EIGEN_SOURCE "FetchContent" )
set( GENERIC_CONTAINER_FKYAML_SOURCE "disabled" )
set( GENERIC_CONTAINER_TOMLPLUSPLUS_SOURCE "disabled" )
set( GENERIC_CONTAINER_LUA_SOURCE "disabled" )

#   _____ _
#  | ____(_) __ _  ___ _ __
#  |  _| | |/ _` |/ _ \ '_ \
#  | |___| | (_| |  __/ | | |
#  |_____|_|\__, |\___|_| |_|
#           |___/
#
# Eigen backs the mat_* types and is a PUBLIC (header-only) dependency of
# the library. Prefer a sibling/local checkout when it looks like a valid
# Eigen source tree, and only fetch the pinned release as a fallback. When
# fetched, SOURCE_SUBDIR points at a directory with no CMakeLists.txt of its
# own ("cmake/", full of .cmake helper scripts but no project file) so
# FetchContent_MakeAvailable() populates the sources without configuring
# Eigen's own heavy, test- and doc-oriented build -- we only need headers.
#
# GLOBAL: without it, this IMPORTED target is only visible inside this
# directory and its subdirectories, not to a parent project that
# FetchContent's GenericContainer -- such a consumer could then not
# target_link_libraries(... Eigen3::Eigen) directly (even though the
# include path still reaches them transitively through GenericContainer's
# own PUBLIC link to it).
if ( NOT TARGET Eigen3::Eigen )
  get_filename_component(
    GENERIC_CONTAINER_EIGEN_LOCAL_DIR_ABS
    "${GENERIC_CONTAINER_EIGEN_LOCAL_DIR}"
    ABSOLUTE
    BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
  )

  if ( EXISTS "${GENERIC_CONTAINER_EIGEN_LOCAL_DIR_ABS}/Eigen/Core" )
    message( STATUS "Using local Eigen checkout: ${GENERIC_CONTAINER_EIGEN_LOCAL_DIR_ABS}" )
    set( GENERIC_CONTAINER_EIGEN_SOURCE "local: ${GENERIC_CONTAINER_EIGEN_LOCAL_DIR_ABS}" )
    add_library( Eigen3::Eigen INTERFACE IMPORTED GLOBAL )
    target_include_directories(
      Eigen3::Eigen SYSTEM INTERFACE
      "${GENERIC_CONTAINER_EIGEN_LOCAL_DIR_ABS}"
    )
  else()
    message( STATUS "Fetching Eigen ${GENERIC_CONTAINER_EIGEN_VERSION}" )
    set( GENERIC_CONTAINER_EIGEN_SOURCE "FetchContent: ${GENERIC_CONTAINER_EIGEN_VERSION}" )
    FetchContent_Declare(
      eigen3
      URL "https://gitlab.com/libeigen/eigen/-/archive/${GENERIC_CONTAINER_EIGEN_VERSION}/eigen-${GENERIC_CONTAINER_EIGEN_VERSION}.tar.gz"
      SOURCE_SUBDIR cmake
    )
    FetchContent_MakeAvailable( eigen3 )
    add_library( Eigen3::Eigen INTERFACE IMPORTED GLOBAL )
    target_include_directories( Eigen3::Eigen SYSTEM INTERFACE "${eigen3_SOURCE_DIR}" )
  endif()
else()
  message( STATUS "Using existing Eigen3::Eigen target" )
  set( GENERIC_CONTAINER_EIGEN_SOURCE "existing target: Eigen3::Eigen" )
endif()

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
target_link_libraries( GenericContainer PUBLIC Eigen3::Eigen )
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
  add_subdirectory( tests )
endif()

#   ___           _        _ _
#  |_ _|_ __  ___| |_ __ _| | |
#   | || '_ \/ __| __/ _` | | |
#   | || | | \__ \ || (_| | | |
#  |___|_| |_|___/\__\__,_|_|_|
#
if ( GENERIC_CONTAINER_INSTALL )
  include( CMakePackageConfigHelpers )

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
  )

  install(
    EXPORT GenericContainerTargets
    FILE GenericContainerTargets.cmake
    NAMESPACE GenericContainer::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GenericContainer
  )

  configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenericContainerConfig.cmake.in"
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
message( STATUS "  Eigen        : ${GENERIC_CONTAINER_EIGEN_SOURCE}" )
message( STATUS "  fkYAML       : ${GENERIC_CONTAINER_FKYAML_SOURCE}" )
message( STATUS "  tomlplusplus : ${GENERIC_CONTAINER_TOMLPLUSPLUS_SOURCE}" )
message( STATUS "  lua          : ${GENERIC_CONTAINER_LUA_SOURCE}" )
message( STATUS "" )
