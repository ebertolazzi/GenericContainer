############################################################################
#                                                                          #
#  GenericContainer -- modern, idiomatic CMake build.                     #
#                                                                          #
#  Covers the core library (src/, include/), its Catch2 test battery      #
#  (tests/), and the optional json/yaml/toml/lua bridges. JSON/YAML/TOML  #
#  are compiled directly into GenericContainer; Lua stays as its own       #
#  runtime library, linked by GenericContainer when the Lua bridge is      #
#  enabled. The platform-specific names are the real library files        #
#  (for example libGenericContainer_osx_static.a and libLua_osx_static.a), #
#  while the generic names are compatibility symlinks/copies.              #
#                                                                          #
############################################################################


# ============================================================================
# Minimum CMake version
# ============================================================================
# This project is written as a target-oriented CMake build: compile features,
# include directories, link dependencies, install rules and export metadata
# are attached to targets instead of being stored in global variables.
# CMake 3.28 is used as the baseline so the script can rely on modern
# generator expressions, FetchContent behavior, install/export support and
# package configuration helpers, including FetchContent's EXCLUDE_FROM_ALL
# support, without compatibility workarounds for older releases.
cmake_minimum_required( VERSION 3.28 )


# --------------------------------------------------------------------------
# Apple FAT/universal build setup
# --------------------------------------------------------------------------
# On Apple platforms a single library can contain slices for several CPU
# architectures, for example arm64 and x86_64.  This is controlled by
# CMAKE_OSX_ARCHITECTURES.
#
# The variable must be set before project(), otherwise CMake may initialize
# compilers and targets before the requested architectures are known.
#
# Default behavior:
#   - ON  on Apple hosts or Apple cross-compilation targets.
#   - OFF on Linux/Windows and other platforms, where there is no equivalent
#     single-file universal library format.
#
# User overrides:
#   -DUTILS_BUILD_FAT=OFF
#   -DUTILS_FAT_ARCHITECTURES=arm64
#   -DCMAKE_OSX_ARCHITECTURES=arm64\;x86_64
set(UTILS_BUILD_FAT_DEFAULT OFF)
if(CMAKE_HOST_APPLE OR CMAKE_SYSTEM_NAME MATCHES "Darwin|iOS|tvOS|watchOS|visionOS")
  set(UTILS_BUILD_FAT_DEFAULT ON)
endif()
option(UTILS_BUILD_FAT "Build Apple universal/fat binaries when supported" ${UTILS_BUILD_FAT_DEFAULT})
set(UTILS_FAT_ARCHITECTURES "arm64;x86_64" CACHE STRING "Apple architectures to build when UTILS_BUILD_FAT is ON")

if(UTILS_BUILD_FAT AND NOT CMAKE_OSX_ARCHITECTURES)
  set(
    CMAKE_OSX_ARCHITECTURES
    "${UTILS_FAT_ARCHITECTURES}"
    CACHE STRING "Build architectures for Apple platforms"
    FORCE
  )
endif()


# Extract the version from a Git tag when available.

# ============================================================================
# Project version derived from Git tags
# ============================================================================
# The build tries to use the latest reachable Git tag as the project version.
# This avoids keeping the version number duplicated in source files and
# release metadata.
# Git is optional. Source archives, shallow checkouts or trees without tags
# still configure deterministically because the fallback version below is
# always available.

# ----------------------------------------------------------------------------
# Locate Git without making it mandatory
# ----------------------------------------------------------------------------
# QUIET suppresses diagnostics when Git is absent or when the source tree is
# not a Git checkout. That is intentional for release archives distributed
# without a .git directory.
find_package(Git QUIET)

# ----------------------------------------------------------------------------
# Read the last reachable tag
# ----------------------------------------------------------------------------
# When Git exists, `git describe --tags --abbrev=0` returns only the nearest
# tag name, for example `v1.2.3` or `1.2.3`. OUTPUT_STRIP_TRAILING_WHITESPACE
# removes the final newline so the result can be passed directly to
# project(VERSION ...).
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

# ----------------------------------------------------------------------------
# Fallback version
# ----------------------------------------------------------------------------
# If Git did not produce a usable tag, use a stable fallback. The value is
# deliberately valid CMake version syntax so package-version files and
# SOVERSION handling still work.
if(NOT GIT_VERSION)
  set(GIT_VERSION "0.0.0")
endif()



# ============================================================================
# Project declaration
# ============================================================================
# project() establishes the public project identity, enables the C++ language,
# initializes variables such as PROJECT_VERSION and PROJECT_IS_TOP_LEVEL, and
# defines metadata reused later by install rules and CPack.
# All toolchain-sensitive settings that must affect compiler detection should
# appear before this call. Target-level build properties can safely appear
# after it.
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

# ============================================================================
# Default build type for single-configuration generators
# ============================================================================
# Unix Makefiles and Ninja build one configuration per build directory, so
# CMAKE_BUILD_TYPE controls optimization/debug flags. When the user did not
# choose a type, default to Release.
# Multi-configuration generators such as Visual Studio, Xcode and Ninja
# Multi-Config ignore CMAKE_BUILD_TYPE; the condition leaves those generators
# untouched.
if ( NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES )
  set( CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE )
  set_property( CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel" )
endif()

# Standalone vs. consumed-as-a-dependency: tests only make sense standalone,
# and installing shouldn't be forced on a project that FetchContent's us.

# ============================================================================
# Top-level user options
# ============================================================================
# These options are stored in the CMake cache and can be overridden with
# -DNAME=ON/OFF from the command line or from a parent project.
# BUILD_SHARED_LIBS is a standard CMake switch. Because the main
# add_library(GenericContainer ...) call does not explicitly say STATIC or
# SHARED, this option controls whether the produced library is static or
# shared.
# GENERIC_CONTAINER_INSTALL defaults to ON only when this is the top-level
# project. That prevents an embedding project from receiving install rules it
# did not ask for.
option( BUILD_SHARED_LIBS "Build GenericContainer as a shared library" OFF )
option( GENERIC_CONTAINER_INSTALL "Generate the install target" ${PROJECT_IS_TOP_LEVEL} )
option( GENERIC_CONTAINER_ALLOW_NETWORK_FETCH "Allow FetchContent network fallbacks for missing local dependencies" OFF )

# Each format bridge is opt-in; turn one off to skip fetching/building its
# third-party dependency entirely.

# ============================================================================
# Optional format bridges
# ============================================================================
# Each bridge adds implementation sources and one external dependency. Turning
# a bridge OFF skips both the extra source files and the corresponding
# dependency lookup/download.
# JSON, YAML and TOML are compiled into the main GenericContainer binary
# target. The namespaced bridge targets remain available as
# interface/compatibility targets for consumers that want to express a
# specific feature dependency.
# Lua is different because it also needs the Lua C runtime. When enabled, the
# build creates a separate Lua library target and links GenericContainer
# against it.
option( GENERIC_CONTAINER_ENABLE_JSON "Build GenericContainer::Json (nlohmann::json-backed)" ON )
option( GENERIC_CONTAINER_ENABLE_YAML "Build GenericContainer::Yaml (fkYAML-backed)" ON )
option( GENERIC_CONTAINER_ENABLE_TOML "Build GenericContainer::Toml (toml++-backed)" ON )
option( GENERIC_CONTAINER_ENABLE_LUA  "Build GenericContainer::Lua (embeds Lua 5.4)"  ON )

# ----------------------------------------------------------------------------
# Examples option
# ----------------------------------------------------------------------------
# Examples are useful for source-tree development but usually undesirable when
# GenericContainer is embedded as a dependency, so the default follows
# PROJECT_IS_TOP_LEVEL.
option( GENERIC_CONTAINER_BUILD_EXAMPLES "Build runnable executables from examples/" ${PROJECT_IS_TOP_LEVEL} )

# CheckIPOSupported is needed by the IPO/LTO probing below.
include( CheckIPOSupported )

# ============================================================================
# Compilation and optimization options
# ============================================================================
# Native CPU tuning is enabled for ordinary single-architecture builds, but
# disabled by default for Apple universal binaries: -march=native cannot
# describe both arm64 and x86_64 slices in the same artifact.
set(GENERIC_CONTAINER_APPLE_MULTI_ARCH FALSE)
if(APPLE AND CMAKE_OSX_ARCHITECTURES)
  list(LENGTH CMAKE_OSX_ARCHITECTURES GENERIC_CONTAINER_OSX_ARCHITECTURE_COUNT)
  if(GENERIC_CONTAINER_OSX_ARCHITECTURE_COUNT GREATER 1)
    set(GENERIC_CONTAINER_APPLE_MULTI_ARCH TRUE)
  endif()
endif()

set(GENERIC_CONTAINER_USE_NATIVE_ARCH_DEFAULT ON)
if(GENERIC_CONTAINER_APPLE_MULTI_ARCH)
  set(GENERIC_CONTAINER_USE_NATIVE_ARCH_DEFAULT OFF)
endif()

option(
  GENERIC_CONTAINER_USE_NATIVE_ARCH
  "Optimize Release builds for the current CPU architecture"
  ${GENERIC_CONTAINER_USE_NATIVE_ARCH_DEFAULT}
)
option(GENERIC_CONTAINER_ENABLE_LTO "Enable interprocedural optimization / LTO" ON)
option(GENERIC_CONTAINER_UNROLL_LOOPS "Enable explicit loop unrolling in Release builds" OFF)
option(
  GENERIC_CONTAINER_ENABLE_FAST_MATH
  "Enable aggressive floating-point optimizations; may change IEEE-754 semantics"
  OFF
)

# Probe IPO/LTO support once and reuse the result for every GenericContainer
# target.
set(GENERIC_CONTAINER_IPO_SUPPORTED FALSE)
set(GENERIC_CONTAINER_IPO_STATUS "disabled")
if(GENERIC_CONTAINER_ENABLE_LTO)
  check_ipo_supported(
    RESULT GENERIC_CONTAINER_IPO_SUPPORTED
    OUTPUT GENERIC_CONTAINER_IPO_ERROR
    LANGUAGES CXX
  )
  if(GENERIC_CONTAINER_IPO_SUPPORTED)
    set(GENERIC_CONTAINER_IPO_STATUS "enabled")
  else()
    set(GENERIC_CONTAINER_IPO_STATUS "not supported: ${GENERIC_CONTAINER_IPO_ERROR}")
  endif()
endif()

# The backend subdirectories use FetchContent for their own dependencies.
# These cache paths let a developer keep sibling checkouts next to this
# project and avoid downloading them again. Set any path to a non-existing
# directory to force the corresponding FetchContent fallback.
set(

# ============================================================================
# Preferred local checkouts for optional dependencies
# ============================================================================
# These cache paths let developers keep dependency repositories next to
# GenericContainer and avoid repeated network downloads. A path is used only
# when its current Git branch matches the configured FetchContent ref;
# otherwise the normal FetchContent fallback remains available.
# Setting a path to a non-existing directory is a simple way to force the
# download/fallback path during testing.
  GENERIC_CONTAINER_NLOHMANN_JSON_LOCAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../json"
  CACHE PATH "Path to a local nlohmann/json checkout to use before FetchContent"
)
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
set(GENERIC_CONTAINER_NLOHMANN_JSON_REF "v3.11.3" CACHE STRING "nlohmann/json branch/tag to fetch")
set(GENERIC_CONTAINER_FKYAML_REF "v0.4.3" CACHE STRING "fkYAML branch/tag to fetch")
set(GENERIC_CONTAINER_TOMLPLUSPLUS_REF "v3.4.0" CACHE STRING "toml++ branch/tag to fetch")
set(GENERIC_CONTAINER_LUA_REF "5.4.6" CACHE STRING "Lua branch/version to fetch")


# ============================================================================
# Standard CMake modules
# ============================================================================
# GNUInstallDirs provides portable install directory variables such as
# CMAKE_INSTALL_INCLUDEDIR, CMAKE_INSTALL_LIBDIR and CMAKE_INSTALL_BINDIR.
# FetchContent is used for dependencies that are not provided by the parent
# project or by find_package().
include( GNUInstallDirs )
include( FetchContent )

# ----------------------------------------------------------------------------
# Persistent FetchContent cache
# ----------------------------------------------------------------------------
# FetchContent normally stores populated dependencies in the current build
# tree. Use a source-tree cache instead so fresh build directories and
# different presets can reuse the same downloads. Preserve any custom path
# explicitly supplied by a parent project or by the user.
if (
  PROJECT_IS_TOP_LEVEL
  AND (
    NOT FETCHCONTENT_BASE_DIR
    OR FETCHCONTENT_BASE_DIR STREQUAL "${CMAKE_BINARY_DIR}/_deps"
  )
)
  set(
    FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/.cache/cmake-deps"
    CACHE PATH "Shared FetchContent download/extract cache" FORCE
  )
endif()

# Once a dependency has been populated, avoid checking its upstream Git
# repository again on every configure. Initial downloads remain enabled.
set(
  FETCHCONTENT_UPDATES_DISCONNECTED ON
  CACHE BOOL "Skip FetchContent update checks after initial population" FORCE
)


# ============================================================================
# Platform suffix used in real library filenames
# ============================================================================
# The project deliberately gives the real binary files platform-specific
# names, for example libGenericContainer_osx_static.a or
# libGenericContainer_linux_static.a.
# A compatibility alias with the traditional unsuffixed name is created later.
# On Unix-like platforms that alias is a symlink; on Windows it is a copied
# file.
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

# ============================================================================
# Dependency-summary variables
# ============================================================================
# These variables record where each optional dependency came from: disabled,
# an existing target, find_package(), a local checkout or FetchContent.
# The final status messages make configuration decisions auditable and help
# diagnose non-reproducible builds.
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

# ============================================================================
# Common target configuration
# ============================================================================
# Apply the same diagnostics, definitions and optimization policy to each
# GenericContainer target.  Flags are target-local (PRIVATE) so they do not
# leak into consumers that link this project.
function( gc_configure_target TARGET_NAME )
  target_compile_options(
    ${TARGET_NAME}
    PRIVATE
      $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall>
      $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wextra>
      $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wpedantic>
      $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wshadow>
      $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wnon-virtual-dtor>
      $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Woverloaded-virtual>
      $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wcast-align>
      $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wunused>
      $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wformat=2>
      $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wimplicit-fallthrough>
      $<$<CXX_COMPILER_ID:GNU>:-Wduplicated-cond>
      $<$<CXX_COMPILER_ID:GNU>:-Wduplicated-branches>
      $<$<CXX_COMPILER_ID:GNU>:-Wlogical-op>
      $<$<CXX_COMPILER_ID:GNU>:-Wnull-dereference>
      $<$<CXX_COMPILER_ID:GNU>:-Wmisleading-indentation>
      $<$<CXX_COMPILER_ID:MSVC>:/W4>
      $<$<CXX_COMPILER_ID:MSVC>:/permissive->
      $<$<CXX_COMPILER_ID:MSVC>:/bigobj>
  )

  target_compile_options(
    ${TARGET_NAME}
    PRIVATE
      $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU,Clang,AppleClang>>:-O3>
      $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>:/O2>
      $<$<AND:$<CONFIG:RelWithDebInfo>,$<CXX_COMPILER_ID:GNU,Clang,AppleClang>>:-O2>
      $<$<AND:$<CONFIG:RelWithDebInfo>,$<CXX_COMPILER_ID:MSVC>>:/O2>
  )

  target_compile_definitions(
    ${TARGET_NAME}
    PRIVATE
      $<$<CONFIG:Release>:NDEBUG>
      $<$<CONFIG:Release>:EIGEN_NO_DEBUG>
  )

  if( GENERIC_CONTAINER_USE_NATIVE_ARCH )
    if( GENERIC_CONTAINER_APPLE_MULTI_ARCH )
      message( STATUS
        "Skipping native CPU flags for ${TARGET_NAME}: Apple universal build uses "
        "CMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}"
      )
    else()
      target_compile_options(
        ${TARGET_NAME}
        PRIVATE
          $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU,Clang,AppleClang>>:-march=native>
          $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU,Clang,AppleClang>>:-mtune=native>
          $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:IntelLLVM>>:-xHost>
          $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>:/arch:AVX2>
      )
    endif()
  endif()

  if( GENERIC_CONTAINER_UNROLL_LOOPS )
    target_compile_options(
      ${TARGET_NAME}
      PRIVATE
        $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU,Clang,AppleClang>>:-funroll-loops>
    )
  endif()

  # Deliberately OFF by default because it permits reassociation and other
  # transformations that weaken strict IEEE-754 behavior.
  if( GENERIC_CONTAINER_ENABLE_FAST_MATH )
    target_compile_options(
      ${TARGET_NAME}
      PRIVATE
        $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU,Clang,AppleClang>>:-ffast-math>
        $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>:/fp:fast>
    )
  endif()

  if( GENERIC_CONTAINER_IPO_SUPPORTED )
    set_property( TARGET ${TARGET_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE ON )
    set_property( TARGET ${TARGET_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON )
  endif()

  if( APPLE AND UTILS_BUILD_FAT )
    set_target_properties(
      ${TARGET_NAME}
      PROPERTIES
        OSX_ARCHITECTURES "${CMAKE_OSX_ARCHITECTURES}"
    )
  endif()
endfunction()

# ============================================================================
# Main library target
# ============================================================================
# GenericContainer is the central binary target. It starts with the core
# implementation sources from src/. Optional bridge sources are appended later
# when their feature options are enabled.
# The library type is intentionally not written as STATIC or SHARED. CMake
# therefore follows BUILD_SHARED_LIBS, which gives users one standard switch
# for both this target and the Lua runtime target.
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

# ----------------------------------------------------------------------------
# Public namespaced alias
# ----------------------------------------------------------------------------
# Consumers should link the namespaced target
# GenericContainer::GenericContainer. It matches the installed package API and
# decouples users from the physical target name used inside this build.
add_library( GenericContainer::GenericContainer ALIAS GenericContainer )

# ----------------------------------------------------------------------------
# Export target accumulator
# ----------------------------------------------------------------------------
# This list is extended when optional exported targets are created. The
# install(EXPORT ...) rule later uses the final list to generate
# GenericContainerTargets.cmake.
set( GENERIC_CONTAINER_EXPORTED_TARGETS GenericContainer )


# ----------------------------------------------------------------------------
# C++ language requirement
# ----------------------------------------------------------------------------
# The PUBLIC scope means GenericContainer itself is compiled as C++20 and
# consumers linking it must also use a C++20-capable compilation mode.
target_compile_features( GenericContainer PUBLIC cxx_std_20 )

# ----------------------------------------------------------------------------
# Public include directories for GenericContainer
# ----------------------------------------------------------------------------
# BUILD_INTERFACE is used while building from the source tree.
# INSTALL_INTERFACE is used by the installed package and keeps the package
# relocatable relative to the installation prefix.
target_include_directories(
  GenericContainer PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)


# ----------------------------------------------------------------------------
# Version and export metadata for GenericContainer
# ----------------------------------------------------------------------------
# VERSION and SOVERSION matter primarily for shared libraries. EXPORT_NAME
# controls the target name written into the installed
# GenericContainerTargets.cmake file.
set_target_properties(
  GenericContainer PROPERTIES
  VERSION   ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  EXPORT_NAME GenericContainer
)


# ----------------------------------------------------------------------------
# Diagnostics, definitions and optimization policy for GenericContainer
# ----------------------------------------------------------------------------
gc_configure_target( GenericContainer )

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

# ============================================================================
# Helper functions for optional backends
# ============================================================================
# The next functions centralize repeated CMake operations: preferring local
# dependency checkouts, maintaining the export list, normalizing output
# filenames, creating compatibility aliases, and ensuring dependency targets
# exist.

# ----------------------------------------------------------------------------
# generic_container_prefer_local_fetchcontent()
# ----------------------------------------------------------------------------
# Parameters: DEPENDENCY_NAME is the FetchContent name expected by the
# backend; LOCAL_DIR is the suggested sibling checkout; SUMMARY_VARIABLE is
# updated in the parent scope for the final report.
# If a suitable local checkout exists, the function sets
# FETCHCONTENT_SOURCE_DIR_<NAME> in the cache.
# FetchContent_MakeAvailable(<name>) will then use that source tree instead of
# downloading it. Stale source overrides are cleared when their Git branch no
# longer matches the requested FetchContent ref.
function( generic_container_prefer_local_fetchcontent DEPENDENCY_NAME LOCAL_DIR EXPECTED_REF SUMMARY_VARIABLE )
  string( TOUPPER "${DEPENDENCY_NAME}" DEPENDENCY_NAME_UPPER )
  set( SOURCE_DIR_VARIABLE "FETCHCONTENT_SOURCE_DIR_${DEPENDENCY_NAME_UPPER}" )
  set( ${SUMMARY_VARIABLE} "FetchContent" PARENT_SCOPE )

  get_filename_component(
    LOCAL_DIR_ABS
    "${LOCAL_DIR}"
    ABSOLUTE
    BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
  )

  set( LOCAL_MATCHES FALSE )
  set( LOCAL_BRANCH "not available" )
  if ( IS_DIRECTORY "${LOCAL_DIR_ABS}" AND GIT_EXECUTABLE )
    execute_process(
      COMMAND "${GIT_EXECUTABLE}" -C "${LOCAL_DIR_ABS}" symbolic-ref --quiet --short HEAD
      RESULT_VARIABLE BRANCH_RESULT
      OUTPUT_VARIABLE LOCAL_BRANCH
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_QUIET
    )
    if ( NOT BRANCH_RESULT EQUAL 0 )
      set( LOCAL_BRANCH "detached HEAD or unavailable" )
    endif()
    if ( BRANCH_RESULT EQUAL 0 AND LOCAL_BRANCH STREQUAL EXPECTED_REF )
      set( LOCAL_MATCHES TRUE )
    endif()
  endif()

  if ( LOCAL_MATCHES )
    message( STATUS "GenericContainer: ${DEPENDENCY_NAME} -> local ${LOCAL_DIR_ABS} (branch '${LOCAL_BRANCH}', requested '${EXPECTED_REF}')" )
    set(
      ${SOURCE_DIR_VARIABLE} "${LOCAL_DIR_ABS}"
      CACHE PATH "Local source directory for ${DEPENDENCY_NAME}; overrides FetchContent download"
      FORCE
    )
    set( ${SUMMARY_VARIABLE} "local: ${LOCAL_DIR_ABS} [branch ${LOCAL_BRANCH}]" PARENT_SCOPE )
  else()
    unset( ${SOURCE_DIR_VARIABLE} CACHE )
    if ( IS_DIRECTORY "${LOCAL_DIR_ABS}" )
      message( STATUS "GenericContainer: ${DEPENDENCY_NAME} -> ignoring local ${LOCAL_DIR_ABS} (branch '${LOCAL_BRANCH}', requested '${EXPECTED_REF}')" )
    else()
      message( STATUS "GenericContainer: ${DEPENDENCY_NAME} -> local checkout not found at ${LOCAL_DIR_ABS}" )
    endif()
    set( ${SUMMARY_VARIABLE} "FetchContent: ${EXPECTED_REF}" PARENT_SCOPE )
  endif()
endfunction()


# ----------------------------------------------------------------------------
# generic_container_append_exported_target()
# ----------------------------------------------------------------------------
# Append a target to the list that will be installed/exported. CMake functions
# have their own scope, so the updated list must be written back to the parent
# scope.
function( generic_container_append_exported_target TARGET_NAME )
  set( updated_exported_targets ${GENERIC_CONTAINER_EXPORTED_TARGETS} )
  list( APPEND updated_exported_targets ${TARGET_NAME} )
  set( GENERIC_CONTAINER_EXPORTED_TARGETS "${updated_exported_targets}" PARENT_SCOPE )
endfunction()


# ----------------------------------------------------------------------------
# generic_container_apply_standard_library_name()
# ----------------------------------------------------------------------------
# Set the conventional output name for a library target. On Windows, the build
# forces a `lib` prefix and disables the automatic prefix properties to
# preserve the historical artifact naming scheme across static and shared
# builds.
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


# ----------------------------------------------------------------------------
# generic_container_add_platform_library_alias()
# ----------------------------------------------------------------------------
# Create two filesystem-level names for the same library: a real
# platform-suffixed file and a standard unsuffixed compatibility name.
# For Unix/macOS builds the compatibility name is a symlink. For Windows
# builds it is copied, because symlink creation can require privileges or
# developer-mode settings. Shared-library Windows builds handle both the
# runtime file (.dll) and the import library (.lib or .dll.a).
function( generic_container_add_platform_library_alias TARGET_NAME BASE_NAME )
  if ( BUILD_SHARED_LIBS )
    if ( WIN32 )
      set( PLATFORM_OUTPUT_NAME "lib${BASE_NAME}_${GENERIC_CONTAINER_PLATFORM_SUFFIX}" )
      set( STANDARD_RUNTIME_FILENAME "lib${BASE_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}" )
      set( STANDARD_IMPORT_FILENAME  "lib${BASE_NAME}${CMAKE_IMPORT_LIBRARY_SUFFIX}" )
      set( PLATFORM_RUNTIME_FILENAME "${PLATFORM_OUTPUT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}" )
      set( PLATFORM_IMPORT_FILENAME  "${PLATFORM_OUTPUT_NAME}${CMAKE_IMPORT_LIBRARY_SUFFIX}" )

      set_target_properties(
        ${TARGET_NAME} PROPERTIES
        OUTPUT_NAME         "${PLATFORM_OUTPUT_NAME}"
        ARCHIVE_OUTPUT_NAME "${PLATFORM_OUTPUT_NAME}"
        LIBRARY_OUTPUT_NAME "${PLATFORM_OUTPUT_NAME}"
        RUNTIME_OUTPUT_NAME "${PLATFORM_OUTPUT_NAME}"
      )

      add_custom_command(
        TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "$<TARGET_FILE:${TARGET_NAME}>"
          "$<TARGET_FILE_DIR:${TARGET_NAME}>/${STANDARD_RUNTIME_FILENAME}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "$<TARGET_LINKER_FILE:${TARGET_NAME}>"
          "$<TARGET_LINKER_FILE_DIR:${TARGET_NAME}>/${STANDARD_IMPORT_FILENAME}"
        VERBATIM
      )

      install(
        FILES "$<TARGET_FILE_DIR:${TARGET_NAME}>/${STANDARD_RUNTIME_FILENAME}"
        DESTINATION ${CMAKE_INSTALL_BINDIR}
      )
      install(
        FILES "$<TARGET_LINKER_FILE_DIR:${TARGET_NAME}>/${STANDARD_IMPORT_FILENAME}"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
      )
    else()
      set( PLATFORM_OUTPUT_NAME "${BASE_NAME}_${GENERIC_CONTAINER_PLATFORM_SUFFIX}" )
      set( STANDARD_FILENAME "${CMAKE_SHARED_LIBRARY_PREFIX}${BASE_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}" )
      set( PLATFORM_FILENAME "${CMAKE_SHARED_LIBRARY_PREFIX}${PLATFORM_OUTPUT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}" )

      set_target_properties(
        ${TARGET_NAME} PROPERTIES
        OUTPUT_NAME         "${PLATFORM_OUTPUT_NAME}"
        ARCHIVE_OUTPUT_NAME "${PLATFORM_OUTPUT_NAME}"
        LIBRARY_OUTPUT_NAME "${PLATFORM_OUTPUT_NAME}"
        RUNTIME_OUTPUT_NAME "${PLATFORM_OUTPUT_NAME}"
      )

      add_custom_command(
        TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E rm -f
          "$<TARGET_LINKER_FILE_DIR:${TARGET_NAME}>/${STANDARD_FILENAME}"
        COMMAND ${CMAKE_COMMAND} -E create_symlink
          "${PLATFORM_FILENAME}"
          "$<TARGET_LINKER_FILE_DIR:${TARGET_NAME}>/${STANDARD_FILENAME}"
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
            \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${STANDARD_FILENAME}\"
        )
        execute_process(
          COMMAND \"${CMAKE_COMMAND}\" -E create_symlink
            \"${PLATFORM_FILENAME}\"
            \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${STANDARD_FILENAME}\"
        )"
      )
    endif()
  else()
    if ( WIN32 )
      set( PLATFORM_OUTPUT_NAME "lib${BASE_NAME}_${GENERIC_CONTAINER_PLATFORM_SUFFIX}_static" )
      set( STANDARD_FILENAME "lib${BASE_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}" )
      set( PLATFORM_FILENAME "${PLATFORM_OUTPUT_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}" )
    else()
      set( PLATFORM_OUTPUT_NAME "${BASE_NAME}_${GENERIC_CONTAINER_PLATFORM_SUFFIX}_static" )
      set( STANDARD_FILENAME "${CMAKE_STATIC_LIBRARY_PREFIX}${BASE_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}" )
      set( PLATFORM_FILENAME "${CMAKE_STATIC_LIBRARY_PREFIX}${PLATFORM_OUTPUT_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}" )
    endif()

    set_target_properties(
      ${TARGET_NAME} PROPERTIES
      OUTPUT_NAME         "${PLATFORM_OUTPUT_NAME}"
      ARCHIVE_OUTPUT_NAME "${PLATFORM_OUTPUT_NAME}"
      LIBRARY_OUTPUT_NAME "${PLATFORM_OUTPUT_NAME}"
      RUNTIME_OUTPUT_NAME "${PLATFORM_OUTPUT_NAME}"
    )

    if ( WIN32 )
      add_custom_command(
        TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "$<TARGET_FILE:${TARGET_NAME}>"
          "$<TARGET_FILE_DIR:${TARGET_NAME}>/${STANDARD_FILENAME}"
        VERBATIM
      )

      install(
        FILES "$<TARGET_FILE_DIR:${TARGET_NAME}>/${STANDARD_FILENAME}"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
      )
    else()
      add_custom_command(
        TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E rm -f
          "$<TARGET_FILE_DIR:${TARGET_NAME}>/${STANDARD_FILENAME}"
        COMMAND ${CMAKE_COMMAND} -E create_symlink
          "${PLATFORM_FILENAME}"
          "$<TARGET_FILE_DIR:${TARGET_NAME}>/${STANDARD_FILENAME}"
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
            \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${STANDARD_FILENAME}\"
        )
        execute_process(
          COMMAND \"${CMAKE_COMMAND}\" -E create_symlink
            \"${PLATFORM_FILENAME}\"
            \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${STANDARD_FILENAME}\"
        )"
      )
    endif()
  endif()
endfunction()


# ----------------------------------------------------------------------------
# generic_container_add_backend_alias()
# ----------------------------------------------------------------------------
# Create an INTERFACE bridge target such as GenericContainerJson plus a
# namespaced alias such as GenericContainer::Json. The target does not build
# its own binary; it links to GenericContainer::GenericContainer and preserves
# a feature-level CMake API.
function( generic_container_add_backend_alias TARGET_NAME EXPORT_NAME )
  add_library( ${TARGET_NAME} INTERFACE )
  add_library( GenericContainer::${EXPORT_NAME} ALIAS ${TARGET_NAME} )
  set_target_properties( ${TARGET_NAME} PROPERTIES EXPORT_NAME ${EXPORT_NAME} )
  target_link_libraries( ${TARGET_NAME} INTERFACE GenericContainer::GenericContainer )

  set( updated_exported_targets ${GENERIC_CONTAINER_EXPORTED_TARGETS} )
  list( APPEND updated_exported_targets ${TARGET_NAME} )
  set( GENERIC_CONTAINER_EXPORTED_TARGETS "${updated_exported_targets}" PARENT_SCOPE )
endfunction()


# ----------------------------------------------------------------------------
# generic_container_ensure_nlohmann_json()
# ----------------------------------------------------------------------------
# Guarantee that nlohmann_json::nlohmann_json exists. The resolution order is:
# reuse an existing target from a parent project, use the vendored single
# header, try find_package(nlohmann_json 3), otherwise fetch the official
# include.zip and create a minimal INTERFACE target.
function( generic_container_ensure_nlohmann_json )
  if ( TARGET nlohmann_json::nlohmann_json )
    set( GENERIC_CONTAINER_NLOHMANN_JSON_SOURCE "existing target: nlohmann_json::nlohmann_json" PARENT_SCOPE )
    return()
  endif()

  if ( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/nlohmann/json.hpp" )
    add_library( nlohmann_json INTERFACE )
    add_library( nlohmann_json::nlohmann_json ALIAS nlohmann_json )
    target_include_directories(
      nlohmann_json SYSTEM INTERFACE
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third_party>"
    )
    set(
      GENERIC_CONTAINER_NLOHMANN_JSON_SOURCE
      "vendored: third_party/nlohmann/json.hpp"
      PARENT_SCOPE
    )
    return()
  endif()

  get_filename_component(
    _nlohmann_local_abs "${GENERIC_CONTAINER_NLOHMANN_JSON_LOCAL_DIR}"
    ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
  )
  set( _nlohmann_local_matches FALSE )
  set( _nlohmann_local_branch "not available" )
  if ( EXISTS "${_nlohmann_local_abs}/single_include/nlohmann/json.hpp" AND GIT_EXECUTABLE )
    execute_process(
      COMMAND "${GIT_EXECUTABLE}" -C "${_nlohmann_local_abs}" symbolic-ref --quiet --short HEAD
      RESULT_VARIABLE _nlohmann_branch_result
      OUTPUT_VARIABLE _nlohmann_local_branch
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_QUIET
    )
    if ( NOT _nlohmann_branch_result EQUAL 0 )
      set( _nlohmann_local_branch "detached HEAD or unavailable" )
    endif()
    if ( _nlohmann_branch_result EQUAL 0 AND _nlohmann_local_branch STREQUAL GENERIC_CONTAINER_NLOHMANN_JSON_REF )
      set( _nlohmann_local_matches TRUE )
    endif()
  endif()
  if ( _nlohmann_local_matches )
    add_library( nlohmann_json INTERFACE )
    add_library( nlohmann_json::nlohmann_json ALIAS nlohmann_json )
    target_include_directories(
      nlohmann_json SYSTEM INTERFACE
      "$<BUILD_INTERFACE:${_nlohmann_local_abs}/single_include>"
    )
    set( GENERIC_CONTAINER_NLOHMANN_JSON_SOURCE "local: ${_nlohmann_local_abs} [branch ${_nlohmann_local_branch}]" PARENT_SCOPE )
    message( STATUS "GenericContainer: nlohmann_json -> local ${_nlohmann_local_abs} (branch '${_nlohmann_local_branch}', requested '${GENERIC_CONTAINER_NLOHMANN_JSON_REF}')" )
    return()
  elseif ( EXISTS "${_nlohmann_local_abs}/single_include/nlohmann/json.hpp" )
    message( STATUS "GenericContainer: nlohmann_json -> ignoring local ${_nlohmann_local_abs} (branch '${_nlohmann_local_branch}', requested '${GENERIC_CONTAINER_NLOHMANN_JSON_REF}')" )
  endif()

  find_package( nlohmann_json 3 QUIET )
  if ( TARGET nlohmann_json::nlohmann_json )
    set( GENERIC_CONTAINER_NLOHMANN_JSON_SOURCE "find_package" PARENT_SCOPE )
    return()
  endif()

  if ( NOT GENERIC_CONTAINER_ALLOW_NETWORK_FETCH )
    message( FATAL_ERROR
      "nlohmann/json was not found locally. Set GENERIC_CONTAINER_NLOHMANN_JSON_LOCAL_DIR "
      "or explicitly enable GENERIC_CONTAINER_ALLOW_NETWORK_FETCH." )
  endif()
  message( STATUS "GenericContainer: fetching nlohmann/json (explicitly enabled)" )
  FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/${GENERIC_CONTAINER_NLOHMANN_JSON_REF}/include.zip
    EXCLUDE_FROM_ALL
  )
  FetchContent_MakeAvailable( nlohmann_json )
  add_library( nlohmann_json INTERFACE )
  add_library( nlohmann_json::nlohmann_json ALIAS nlohmann_json )
  target_include_directories(
    nlohmann_json SYSTEM INTERFACE
    "$<BUILD_INTERFACE:${nlohmann_json_SOURCE_DIR}/include>"
  )
  set( GENERIC_CONTAINER_NLOHMANN_JSON_SOURCE "FetchContent: ${GENERIC_CONTAINER_NLOHMANN_JSON_REF}" PARENT_SCOPE )
endfunction()


# ----------------------------------------------------------------------------
# Prepare optional dependencies
# ----------------------------------------------------------------------------
# Before adding bridge sources, prepare the dependencies they need. JSON
# ensures its target immediately; YAML, TOML and Lua set up local-checkout
# overrides before the later FetchContent_MakeAvailable() calls.
if ( GENERIC_CONTAINER_ENABLE_JSON )
  generic_container_ensure_nlohmann_json()
endif()

# ----------------------------------------------------------------------------
# Prepare fkYAML local override
# ----------------------------------------------------------------------------
# If YAML support is enabled, prefer a local fkYAML checkout when one is
# available; otherwise the backend block below will use find_package() or
# FetchContent.
if ( GENERIC_CONTAINER_ENABLE_YAML )
  generic_container_prefer_local_fetchcontent( fkYAML "${GENERIC_CONTAINER_FKYAML_LOCAL_DIR}" "${GENERIC_CONTAINER_FKYAML_REF}" GENERIC_CONTAINER_FKYAML_SOURCE )
endif()

# ----------------------------------------------------------------------------
# Prepare toml++ local override
# ----------------------------------------------------------------------------
# If TOML support is enabled, prefer a local toml++ checkout when one is
# available; otherwise the backend block below resolves it normally.
if ( GENERIC_CONTAINER_ENABLE_TOML )
  generic_container_prefer_local_fetchcontent( tomlplusplus "${GENERIC_CONTAINER_TOMLPLUSPLUS_LOCAL_DIR}" "${GENERIC_CONTAINER_TOMLPLUSPLUS_REF}" GENERIC_CONTAINER_TOMLPLUSPLUS_SOURCE )
endif()

# ----------------------------------------------------------------------------
# Prepare Lua local override
# ----------------------------------------------------------------------------
# If Lua support is enabled, prefer a local Lua source tree when one is
# available. The actual Lua runtime target is created later because it also
# requires enabling the C language.
if ( GENERIC_CONTAINER_ENABLE_LUA )
  generic_container_prefer_local_fetchcontent( lua54 "${GENERIC_CONTAINER_LUA_LOCAL_DIR}" "${GENERIC_CONTAINER_LUA_REF}" GENERIC_CONTAINER_LUA_SOURCE )
endif()


# ----------------------------------------------------------------------------
# Apply output naming to the main library
# ----------------------------------------------------------------------------
# First set the conventional name, then replace the real output with the
# platform-suffixed name and create the compatibility alias.
generic_container_apply_standard_library_name( GenericContainer GenericContainer )
generic_container_add_platform_library_alias( GenericContainer GenericContainer )


# ----------------------------------------------------------------------------
# JSON bridge
# ----------------------------------------------------------------------------
# Add the JSON implementation to the main library and link nlohmann_json for
# the build tree. The GenericContainer::Json alias lets consumers express a
# dependency on the JSON feature explicitly.
if ( GENERIC_CONTAINER_ENABLE_JSON )
  target_sources( GenericContainer PRIVATE interfaces/json/GenericContainerJson.cc )
  target_link_libraries( GenericContainer PRIVATE $<BUILD_INTERFACE:nlohmann_json::nlohmann_json> )
  generic_container_add_backend_alias( GenericContainerJson Json )
endif()

# ----------------------------------------------------------------------------
# YAML bridge
# ----------------------------------------------------------------------------
# Look for an installed fkYAML package first. If none is available,
# FetchContent obtains the requested upstream release archive. An archive is
# used instead of a mutable Git clone because fkYAML changed the case of its
# Catch2 directory between releases, which makes Git tag switches unreliable
# on case-insensitive filesystems such as the default macOS filesystem.
# The YAML conversion sources are compiled into the main library.
if ( GENERIC_CONTAINER_ENABLE_YAML )
  if ( NOT FETCHCONTENT_SOURCE_DIR_FKYAML )
    find_package( fkYAML 0.4 QUIET )
    if ( TARGET fkYAML::fkYAML )
      set( GENERIC_CONTAINER_FKYAML_SOURCE "find_package" )
    endif()
  endif()
  if ( NOT TARGET fkYAML::fkYAML )
    if ( NOT GENERIC_CONTAINER_ALLOW_NETWORK_FETCH AND NOT FETCHCONTENT_SOURCE_DIR_FKYAML )
      message( FATAL_ERROR "fkYAML '${GENERIC_CONTAINER_FKYAML_REF}' requires FetchContent because the local branch does not match; enable GENERIC_CONTAINER_ALLOW_NETWORK_FETCH." )
    endif()
    if ( NOT FETCHCONTENT_SOURCE_DIR_FKYAML )
      message( STATUS "GenericContainer: fkYAML -> FetchContent (requested '${GENERIC_CONTAINER_FKYAML_REF}')" )
    endif()
    FetchContent_Declare(
      fkYAML
      URL "https://github.com/fktn-k/fkYAML/archive/refs/tags/${GENERIC_CONTAINER_FKYAML_REF}.tar.gz"
      SOURCE_DIR "${FETCHCONTENT_BASE_DIR}/fkyaml-${GENERIC_CONTAINER_FKYAML_REF}-src"
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE
      EXCLUDE_FROM_ALL
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

# ----------------------------------------------------------------------------
# TOML bridge
# ----------------------------------------------------------------------------
# Resolve toml++ with find_package() first and FetchContent second. The
# requested package version matches the fallback tag used when no package
# exists.
if ( GENERIC_CONTAINER_ENABLE_TOML )
  if ( NOT FETCHCONTENT_SOURCE_DIR_TOMLPLUSPLUS )
    find_package( tomlplusplus 3.4 QUIET )
    if ( TARGET tomlplusplus::tomlplusplus )
      set( GENERIC_CONTAINER_TOMLPLUSPLUS_SOURCE "find_package" )
    endif()
  endif()
  if ( NOT TARGET tomlplusplus::tomlplusplus )
    if ( NOT GENERIC_CONTAINER_ALLOW_NETWORK_FETCH AND NOT FETCHCONTENT_SOURCE_DIR_TOMLPLUSPLUS )
      message( FATAL_ERROR "toml++ '${GENERIC_CONTAINER_TOMLPLUSPLUS_REF}' requires FetchContent because the local branch does not match; enable GENERIC_CONTAINER_ALLOW_NETWORK_FETCH." )
    endif()
    if ( NOT FETCHCONTENT_SOURCE_DIR_TOMLPLUSPLUS )
      message( STATUS "GenericContainer: tomlplusplus -> FetchContent (requested '${GENERIC_CONTAINER_TOMLPLUSPLUS_REF}')" )
    endif()
    FetchContent_Declare(
      tomlplusplus
      GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
      GIT_TAG        ${GENERIC_CONTAINER_TOMLPLUSPLUS_REF}
      GIT_SHALLOW    TRUE
      EXCLUDE_FROM_ALL
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

# ----------------------------------------------------------------------------
# Lua bridge and Lua 5.4 runtime
# ----------------------------------------------------------------------------
# Lua support is the only bridge that builds an additional runtime library.
# The Lua VM is compiled from C sources, exported as GenericContainer::LuaVM,
# and linked publicly by GenericContainer when the bridge is enabled.
if ( GENERIC_CONTAINER_ENABLE_LUA )
  enable_language( C )

  if ( NOT GENERIC_CONTAINER_ALLOW_NETWORK_FETCH AND NOT FETCHCONTENT_SOURCE_DIR_LUA54 )
    message( FATAL_ERROR "Lua '${GENERIC_CONTAINER_LUA_REF}' requires FetchContent because the local branch does not match; enable GENERIC_CONTAINER_ALLOW_NETWORK_FETCH." )
  endif()
  if ( NOT FETCHCONTENT_SOURCE_DIR_LUA54 )
    message( STATUS "GenericContainer: lua54 -> FetchContent (requested '${GENERIC_CONTAINER_LUA_REF}')" )
  endif()

  FetchContent_Declare(
    lua54
    URL https://www.lua.org/ftp/lua-${GENERIC_CONTAINER_LUA_REF}.tar.gz
    SOURCE_SUBDIR doc
    EXCLUDE_FROM_ALL
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


# ============================================================================
# Platform suffix used in real library filenames
# ============================================================================
# The project deliberately gives the real binary files platform-specific
# names, for example libGenericContainer_osx_static.a or
# libGenericContainer_linux_static.a.
# A compatibility alias with the traditional unsuffixed name is created later.
# On Unix-like platforms that alias is a symlink; on Windows it is a copied
# file.
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

# ============================================================================
# Unit tests
# ============================================================================
# CTest defines the BUILD_TESTING option and provides add_test() integration.
# Tests are built only when this repository is the top-level project, which
# prevents a parent project from unexpectedly downloading Catch2 or compiling
# GenericContainer tests.
include( CTest )

# ----------------------------------------------------------------------------
# Top-level test gate
# ----------------------------------------------------------------------------
# Only a standalone checkout builds the test battery. This keeps FetchContent
# users from inheriting test executables or Catch2 downloads.
if ( BUILD_TESTING AND PROJECT_IS_TOP_LEVEL )
  find_package( Catch2 3 QUIET )
  if ( NOT TARGET Catch2::Catch2WithMain )
    FetchContent_Declare(
      Catch2
      GIT_REPOSITORY https://github.com/catchorg/Catch2.git
      GIT_TAG        v3.7.1
      GIT_SHALLOW    TRUE
      EXCLUDE_FROM_ALL
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

# ============================================================================
# Example executables
# ============================================================================
# Examples are built only in a top-level source checkout and only when the
# examples option is enabled. Executables are written to the source-tree bin/
# directory to preserve the project’s historical layout.
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

# ============================================================================
# Install rules and exported CMake package
# ============================================================================
# This section creates an installation that can be consumed with
# find_package(GenericContainer CONFIG REQUIRED). It installs headers, binary
# targets and generated package metadata under the standard GNUInstallDirs
# locations.
# The whole section is gated by GENERIC_CONTAINER_INSTALL so an embedded build
# can opt out cleanly.
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

# ============================================================================
# Configuration summary
# ============================================================================
# The final status messages print the dependency resolution decisions made
# during configure. This is intentionally at the end, after all optional
# backends had a chance to update their summary variables.
message( STATUS "" )
message( STATUS "GenericContainer dependency summary:" )
message( STATUS "  nlohmann_json: ${GENERIC_CONTAINER_NLOHMANN_JSON_SOURCE}" )
message( STATUS "  fkYAML       : ${GENERIC_CONTAINER_FKYAML_SOURCE}" )
message( STATUS "  tomlplusplus : ${GENERIC_CONTAINER_TOMLPLUSPLUS_SOURCE}" )
message( STATUS "  lua          : ${GENERIC_CONTAINER_LUA_SOURCE}" )
message( STATUS "  Compiler         : ${CMAKE_CXX_COMPILER_ID}" )
message( STATUS "  Build shared     : ${BUILD_SHARED_LIBS}" )
message( STATUS "  Native arch      : ${GENERIC_CONTAINER_USE_NATIVE_ARCH}" )
message( STATUS "  Apple multi-arch : ${GENERIC_CONTAINER_APPLE_MULTI_ARCH}" )
message( STATUS "  LTO requested    : ${GENERIC_CONTAINER_ENABLE_LTO}" )
message( STATUS "  LTO status       : ${GENERIC_CONTAINER_IPO_STATUS}" )
message( STATUS "  Loop unrolling   : ${GENERIC_CONTAINER_UNROLL_LOOPS}" )
message( STATUS "  Fast math        : ${GENERIC_CONTAINER_ENABLE_FAST_MATH}" )

set(GENERIC_CONTAINER_EFFECTIVE_OPTIMIZATION_FLAGS)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
  list(APPEND GENERIC_CONTAINER_EFFECTIVE_OPTIMIZATION_FLAGS "Release:-O3" "RelWithDebInfo:-O2")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  list(APPEND GENERIC_CONTAINER_EFFECTIVE_OPTIMIZATION_FLAGS "Release:/O2" "RelWithDebInfo:/O2")
endif()
if(GENERIC_CONTAINER_USE_NATIVE_ARCH)
  if(GENERIC_CONTAINER_APPLE_MULTI_ARCH)
    list(APPEND GENERIC_CONTAINER_EFFECTIVE_OPTIMIZATION_FLAGS "native-arch:disabled-for-Apple-universal-build")
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    list(APPEND GENERIC_CONTAINER_EFFECTIVE_OPTIMIZATION_FLAGS "Release:-march=native" "Release:-mtune=native")
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM")
    list(APPEND GENERIC_CONTAINER_EFFECTIVE_OPTIMIZATION_FLAGS "Release:-xHost")
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    list(APPEND GENERIC_CONTAINER_EFFECTIVE_OPTIMIZATION_FLAGS "Release:/arch:AVX2")
  endif()
endif()
if(GENERIC_CONTAINER_UNROLL_LOOPS AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
  list(APPEND GENERIC_CONTAINER_EFFECTIVE_OPTIMIZATION_FLAGS "Release:-funroll-loops")
endif()
if(GENERIC_CONTAINER_ENABLE_FAST_MATH)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    list(APPEND GENERIC_CONTAINER_EFFECTIVE_OPTIMIZATION_FLAGS "Release:-ffast-math")
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    list(APPEND GENERIC_CONTAINER_EFFECTIVE_OPTIMIZATION_FLAGS "Release:/fp:fast")
  endif()
endif()
if(GENERIC_CONTAINER_IPO_SUPPORTED)
  list(APPEND GENERIC_CONTAINER_EFFECTIVE_OPTIMIZATION_FLAGS "Release/RelWithDebInfo:IPO/LTO")
endif()
list(APPEND GENERIC_CONTAINER_EFFECTIVE_OPTIMIZATION_FLAGS "Release:NDEBUG" "Release:EIGEN_NO_DEBUG")
list(JOIN GENERIC_CONTAINER_EFFECTIVE_OPTIMIZATION_FLAGS " " GENERIC_CONTAINER_EFFECTIVE_OPTIMIZATION_FLAGS_TEXT)
message( STATUS "  Optimization flags: ${GENERIC_CONTAINER_EFFECTIVE_OPTIMIZATION_FLAGS_TEXT}" )
message( STATUS "" )
