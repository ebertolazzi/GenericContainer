############################################################################
#                                                                          #
#  YAML support for GenericContainer, built as a separate opt-in library   #
#  (GenericContainer::Yaml) rather than linked into the core library, so   #
#  the core stays self-contained/standalone-linkable regardless of        #
#  whether a consumer needs YAML at all.                                  #
#                                                                          #
############################################################################

find_package( fkYAML 0.4 QUIET )
if ( NOT TARGET fkYAML::fkYAML )
  FetchContent_Declare(
    fkYAML
    GIT_REPOSITORY https://github.com/fktn-k/fkYAML.git
    GIT_TAG        v0.4.2
  )
  FetchContent_MakeAvailable( fkYAML )
endif()

add_library( GenericContainerYaml from_yaml.cc to_yaml.cc )
add_library( GenericContainer::Yaml ALIAS GenericContainerYaml )
# So the installed/exported name (GenericContainer::Yaml) matches this
# build-tree alias exactly, instead of the default GenericContainer::GenericContainerYaml.
set_target_properties( GenericContainerYaml PROPERTIES EXPORT_NAME Yaml )

target_compile_features( GenericContainerYaml PUBLIC cxx_std_20 )
target_link_libraries(
  GenericContainerYaml
  PUBLIC  GenericContainer::GenericContainer
  # $<BUILD_INTERFACE:...>: fkYAML is header-only and never appears in this
  # library's own public headers (confirmed: GenericContainerInterface_yaml.hh
  # only calls the from_yaml()/to_yaml() core methods implemented here), so
  # its effects are fully baked into this library's compiled object code --
  # nothing needs to survive into the INSTALL_INTERFACE. Without this wrap,
  # install(EXPORT ...) would refuse to export GenericContainerYaml, since
  # fkYAML::fkYAML (found or fetched locally, never installed by us) wouldn't
  # be resolvable from an installed GenericContainerTargets.cmake.
  PRIVATE $<BUILD_INTERFACE:fkYAML::fkYAML>
)
