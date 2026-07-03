############################################################################
#                                                                          #
#  Lua support for GenericContainer, built as a separate opt-in library    #
#  (GenericContainer::Lua) rather than linked into the core library, so    #
#  the core stays self-contained/standalone-linkable regardless of        #
#  whether a consumer needs Lua at all.                                   #
#                                                                          #
#  Lua ships no CMake build of its own (a plain Makefile), so it's         #
#  fetched as a source tarball and built by hand here: SOURCE_SUBDIR       #
#  points at a directory with no CMakeLists.txt of its own ("doc/", only   #
#  manual pages) so FetchContent_MakeAvailable() populates the sources     #
#  without trying to add_subdirectory() a non-CMake project.               #
#                                                                          #
############################################################################

# The top-level project() only declares LANGUAGES CXX; Lua is plain C, so
# enable the C compiler here, scoped to only running when this optional
# backend is actually built.
enable_language( C )

FetchContent_Declare(
  lua54
  URL https://www.lua.org/ftp/lua-5.4.6.tar.gz
  SOURCE_SUBDIR doc
)
FetchContent_MakeAvailable( lua54 )

file( GLOB LUA_SOURCES CONFIGURE_DEPENDS "${lua54_SOURCE_DIR}/src/*.c" )
# lua.c/luac.c are the standalone interpreter/compiler executables' own
# main()s, not part of the embeddable library.
list( FILTER LUA_SOURCES EXCLUDE REGEX "/lua\\.c$" )
list( FILTER LUA_SOURCES EXCLUDE REGEX "/luac\\.c$" )

# Unlike toml++/fkYAML, Lua has real compiled code that must reach whatever
# finally links GenericContainer::Lua -- there's no header-only trick to
# hide it behind. So, unlike those two, "lua" IS installed and exported
# (see the root CMakeLists.txt), under EXPORT_NAME LuaVM to keep it visibly
# distinct from GenericContainer::Lua (the actual from_lua/to_lua bridge
# this directory builds) once both land in the GenericContainer:: namespace.
add_library( lua STATIC ${LUA_SOURCES} )
add_library( lua::lua ALIAS lua )
set_target_properties( lua PROPERTIES EXPORT_NAME LuaVM )
target_include_directories(
  lua SYSTEM PUBLIC
  "$<BUILD_INTERFACE:${lua54_SOURCE_DIR}/src>"
)

if ( APPLE )
  target_compile_definitions( lua PRIVATE LUA_USE_MACOSX )
elseif ( UNIX )
  target_compile_definitions( lua PRIVATE LUA_USE_LINUX )
  target_link_libraries( lua PUBLIC m ${CMAKE_DL_LIBS} )
endif()

add_library( GenericContainerLua GenericContainerInterface_lua.cc GenericContainerLuaPmain.cc )
add_library( GenericContainer::Lua ALIAS GenericContainerLua )
# So the installed/exported name (GenericContainer::Lua) matches this
# build-tree alias exactly, instead of the default GenericContainer::GenericContainerLua.
set_target_properties( GenericContainerLua PROPERTIES EXPORT_NAME Lua )

target_compile_features( GenericContainerLua PUBLIC cxx_std_20 )
target_link_libraries(
  GenericContainerLua
  PUBLIC  GenericContainer::GenericContainer
  PRIVATE lua::lua
)
