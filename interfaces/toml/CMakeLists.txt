############################################################################
#                                                                          #
#  TOML support for GenericContainer, built as a separate opt-in library   #
#  (GenericContainer::Toml) rather than linked into the core library, so   #
#  the core stays self-contained/standalone-linkable regardless of        #
#  whether a consumer needs TOML at all.                                  #
#                                                                          #
############################################################################

find_package( tomlplusplus 3.4 QUIET )
if ( NOT TARGET tomlplusplus::tomlplusplus )
  FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG        v3.4.0
  )
  FetchContent_MakeAvailable( tomlplusplus )
endif()

add_library( GenericContainerToml from_toml.cc to_toml.cc )
add_library( GenericContainer::Toml ALIAS GenericContainerToml )
# So the installed/exported name (GenericContainer::Toml) matches this
# build-tree alias exactly, instead of the default GenericContainer::GenericContainerToml.
set_target_properties( GenericContainerToml PROPERTIES EXPORT_NAME Toml )

target_compile_features( GenericContainerToml PUBLIC cxx_std_20 )
target_link_libraries(
  GenericContainerToml
  PUBLIC  GenericContainer::GenericContainer
  # $<BUILD_INTERFACE:...>: toml++ is header-only and never appears in this
  # library's own public headers (confirmed: GenericContainerInterface_toml.hh
  # only calls the from_toml()/to_toml() core methods implemented here), so
  # its effects are fully baked into this library's compiled object code --
  # nothing needs to survive into the INSTALL_INTERFACE. Without this wrap,
  # install(EXPORT ...) would refuse to export GenericContainerToml, since
  # tomlplusplus::tomlplusplus (found or fetched locally, never installed by
  # us) wouldn't be resolvable from an installed GenericContainerTargets.cmake.
  PRIVATE $<BUILD_INTERFACE:tomlplusplus::tomlplusplus>
)
