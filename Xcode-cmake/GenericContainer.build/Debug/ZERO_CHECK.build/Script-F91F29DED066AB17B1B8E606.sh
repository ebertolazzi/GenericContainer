#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/Xcode-cmake
  make -f /Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/Xcode-cmake/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/Xcode-cmake
  make -f /Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/Xcode-cmake/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/Xcode-cmake
  make -f /Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/Xcode-cmake/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/Xcode-cmake
  make -f /Users/enrico/Ricerca/develop/C++/pins-mechatronix/LibSources/submodules/GC/Xcode-cmake/CMakeScripts/ReRunCMake.make
fi

