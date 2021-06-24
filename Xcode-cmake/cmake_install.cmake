# Install script for directory: /Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/objdump")
endif()

set(CMAKE_BINARY_DIR "/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/Xcode-cmake")

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib/lib/libGenericContainer_osx_static.a")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
file(INSTALL DESTINATION "/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib/lib" TYPE STATIC_LIBRARY MESSAGE_ALWAYS FILES "/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/Xcode-cmake/Debug/libGenericContainer_osx_static.a")
    if(EXISTS "$ENV{DESTDIR}/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib/lib/libGenericContainer_osx_static.a" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib/lib/libGenericContainer_osx_static.a")
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib/lib/libGenericContainer_osx_static.a")
    endif()
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib/lib/libGenericContainer_osx_static.a")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
file(INSTALL DESTINATION "/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib/lib" TYPE STATIC_LIBRARY MESSAGE_ALWAYS FILES "/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/Xcode-cmake/Release/libGenericContainer_osx_static.a")
    if(EXISTS "$ENV{DESTDIR}/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib/lib/libGenericContainer_osx_static.a" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib/lib/libGenericContainer_osx_static.a")
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib/lib/libGenericContainer_osx_static.a")
    endif()
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib/lib/libGenericContainer_osx_static.a")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
file(INSTALL DESTINATION "/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib/lib" TYPE STATIC_LIBRARY MESSAGE_ALWAYS FILES "/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/Xcode-cmake/MinSizeRel/libGenericContainer_osx_static.a")
    if(EXISTS "$ENV{DESTDIR}/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib/lib/libGenericContainer_osx_static.a" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib/lib/libGenericContainer_osx_static.a")
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib/lib/libGenericContainer_osx_static.a")
    endif()
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib/lib/libGenericContainer_osx_static.a")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
file(INSTALL DESTINATION "/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib/lib" TYPE STATIC_LIBRARY MESSAGE_ALWAYS FILES "/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/Xcode-cmake/RelWithDebInfo/libGenericContainer_osx_static.a")
    if(EXISTS "$ENV{DESTDIR}/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib/lib/libGenericContainer_osx_static.a" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib/lib/libGenericContainer_osx_static.a")
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib/lib/libGenericContainer_osx_static.a")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib/include/GenericContainer.hh;/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib/include/GenericContainerConfig.hh;/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib/include/GenericContainerLibs.hh;/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib/include/GenericContainerCinterface.h;/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib/include/GenericContainerJson.hh;/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib/include/GenericContainerJsonHandler.hh;/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib/include/GenericContainerLuaInterface.hh")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/lib/include" TYPE FILE MESSAGE_ALWAYS FILES
    "/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/src/GenericContainer.hh"
    "/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/src/GenericContainerConfig.hh"
    "/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/src/GenericContainerLibs.hh"
    "/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/src/GenericContainerCinterface.h"
    "/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/src_json_interface/GenericContainerJson.hh"
    "/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/src_json_interface/GenericContainerJsonHandler.hh"
    "/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/src_lua_interface/GenericContainerLuaInterface.hh"
    )
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/Xcode-cmake/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
