############################################################################
#                                                                          #
#  Catch2 v3 unit-test battery for GenericContainer.                       #
#                                                                          #
#  Included from the top-level CMakeLists.txt only when BUILD_TESTING is  #
#  on and GenericContainer is the top-level project.                      #
#                                                                          #
############################################################################

find_package( Catch2 3 QUIET )
if ( NOT TARGET Catch2::Catch2WithMain )
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.7.1
  )
  FetchContent_MakeAvailable( Catch2 )

  # Catch2's own sources are compiled with this project's warning flags
  # (CMAKE_CXX_FLAGS/add_compile_options reach fetched subdirectories too);
  # silence them for the vendored targets only, since we don't maintain
  # that code. Catch2's CMakeLists only requires cxx_std_14 for itself,
  # which leaves its std::string_view StringMaker feature-detected out
  # (its C++17-conditional bits compile against whatever standard its own
  # TUs see) -- bump it to match ours so the symbols the C++20 test TUs
  # expect actually get compiled into libCatch2.
  foreach( catch2_target Catch2 Catch2WithMain )
    if ( TARGET ${catch2_target} )
      target_compile_options( ${catch2_target} PRIVATE $<IF:$<CXX_COMPILER_ID:MSVC>,/w,-w> )
      target_compile_features( ${catch2_target} PUBLIC cxx_std_20 )
    endif()
  endforeach()

  # An installed Catch2 package registers its own "extras/" module path
  # (Catch.cmake, catch_discover_tests) via its Config file; the fetched
  # copy has no such config, so wire it up by hand.
  list( APPEND CMAKE_MODULE_PATH "${Catch2_SOURCE_DIR}/extras" )
endif()

# nlohmann/json is only needed to build test_nlohmann_interface.cc, which
# exercises the opt-in include/GenericContainer/GenericContainerInterface_nlohmann.hh
# adapter; GenericContainer itself has no dependency on it. Prefer an
# installed package, fetch the header-only "include.zip" release asset
# otherwise (no CMakeLists.txt in that archive, so nothing to build).
find_package( nlohmann_json 3 QUIET )
if ( NOT TARGET nlohmann_json::nlohmann_json )
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
endif()

file( GLOB GC_TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/test_*.cc" )

# test_ffi.cc, test_tables.cc, test_support.cc and test_json_yaml_toml.cc
# exercise from_json/to_json/from_yaml/to_yaml/from_toml/to_toml/from_file,
# whose implementations live under src_*_interface/ -- not part of this
# build yet (see the top-level CMakeLists.txt header comment). Drop them
# until that build is restructured too; re-add once GenericContainer links
# against those backends again.
list(
  REMOVE_ITEM GC_TEST_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/test_ffi.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_tables.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_support.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_json_yaml_toml.cc"
)

add_executable( gc_unit_tests ${GC_TEST_SOURCES} )
target_link_libraries(
  gc_unit_tests PRIVATE
  GenericContainer::GenericContainer
  nlohmann_json::nlohmann_json
  Catch2::Catch2WithMain
)

# Tests resolve fixture files (examples/*.json, ...) relative to the repo root.
include( Catch )
catch_discover_tests( gc_unit_tests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} )
