# get the type of OS currently running
OS   = $(shell uname)
CC   = gcc
CXX  = g++ -std=c++11
INC += -I../lib/include
LIBS = -llua -lstdc++ -lm

# check if the OS string contains 'Linux'
ifneq (,$(findstring Linux, $(OS)))
  WARN = -Wall
  # activate C++11 for g++ >= 4.9
  VERSION  = $(shell $(CC) -dumpversion)
  CXXFLAGS = -pthread -O2 -g -funroll-loops -fPIC
  CFLAGS   = -pthread -O2 -g -funroll-loops -fPIC
  CC      += $(WARN)
  CXX     += $(WARN)
  AR       = ar rcs
endif

# check if the OS string contains 'Darwin'
ifneq (,$(findstring Darwin, $(OS)))
  WARN     = -Weverything -Wno-reserved-id-macro -Wno-padded
  CC       = clang
  CXX      = clang++ -std=c++11 -stdlib=libc++
  VERSION  = $(shell $(CC) --version 2>&1 | grep -o "Apple LLVM version [0-9]\.[0-9]\.[0-9]" | grep -o " [0-9]\.")
  CC      += $(WARN)
  CXX     += $(WARN)
  CXXFLAGS = -O2 -g -funroll-loops -fPIC
  CXXFLAGS = -O2 -g -funroll-loops -fPIC
  AR       = libtool -static -o
endif

# prefix for installation, use make PREFIX=/new/prefix install
# to override
PREFIX    = /opt/local
FRAMEWORK = GenericContainer

INC     += -I./src
LIB_DIR  = -L./lib
DEFINE   =
SRCS     = \
../src/GenericContainer.cc \
../src/GenericContainerSupport.cc \
../src/GenericContainerTables.cc \
GenericContainerLuaInterface.cc

MKDIR  = mkdir -p

all:
	$(CXX) $(CXXFLAGS) $(INC) -o example1 example_lua1.cc $(SRCS) $(LIBS)
	$(CXX) $(CXXFLAGS) $(INC) -o example2 example_lua2.cc $(SRCS) $(LIBS)
	$(CXX) $(CXXFLAGS) $(INC) -o example3 example_lua3.cc $(SRCS) $(LIBS)

clean:
	rm -f example1
	rm -f example2
