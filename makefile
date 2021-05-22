# get the type of OS currently running
OS      = $(shell uname)
LIB_GC  = libGenericContainer.a
CC      = gcc
CXX     = g++ -std=c++11
INC    += -I./lib3rd/include -I./src_json_interface  -I./src_lua_interface
LIBSGCC = -lstdc++ -lm

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
  WARN     = -Wall -Wno-reserved-id-macro -Wno-padded
  CC       = clang
  CXX      = clang++ -std=c++11 -stdlib=libc++
  VERSION  = $(shell $(CC) --version 2>&1 | grep -o "Apple LLVM version [0-9]\.[0-9]\.[0-9]" | grep -o " [0-9]\.")
  CC      += $(WARN)
  CXX     += $(WARN)
  CXXFLAGS = -O2 -g -funroll-loops -fPIC
  CXXFLAGS = -O2 -g -funroll-loops -fPIC
  AR       = libtool -static -o
endif

SRCS = \
src/GenericContainer.cc \
src/GenericContainerCinterface.cc \
src/GenericContainerSupport.cc \
src/GenericContainerTables.cc \
src_json_interface/GenericContainerJson.cc \
src_json_interface/GenericContainerJsonHandler.cc \
src_lua_interface/GenericContainerLuaInterface.cc \
src_lua_interface/GenericContainerLuaPmain.cc

SRCS_LUA = \
src_lua_interface/lua-5.4.3/src/lapi.c \
src_lua_interface/lua-5.4.3/src/lauxlib.c \
src_lua_interface/lua-5.4.3/src/lbaselib.c \
src_lua_interface/lua-5.4.3/src/lcode.c \
src_lua_interface/lua-5.4.3/src/lcorolib.c \
src_lua_interface/lua-5.4.3/src/lctype.c \
src_lua_interface/lua-5.4.3/src/ldblib.c \
src_lua_interface/lua-5.4.3/src/ldebug.c \
src_lua_interface/lua-5.4.3/src/ldo.c \
src_lua_interface/lua-5.4.3/src/ldump.c \
src_lua_interface/lua-5.4.3/src/lfunc.c \
src_lua_interface/lua-5.4.3/src/lgc.c \
src_lua_interface/lua-5.4.3/src/linit.c \
src_lua_interface/lua-5.4.3/src/liolib.c \
src_lua_interface/lua-5.4.3/src/llex.c \
src_lua_interface/lua-5.4.3/src/lmathlib.c \
src_lua_interface/lua-5.4.3/src/lmem.c \
src_lua_interface/lua-5.4.3/src/loadlib.c \
src_lua_interface/lua-5.4.3/src/lobject.c \
src_lua_interface/lua-5.4.3/src/lopcodes.c \
src_lua_interface/lua-5.4.3/src/loslib.c \
src_lua_interface/lua-5.4.3/src/lparser.c \
src_lua_interface/lua-5.4.3/src/lstate.c \
src_lua_interface/lua-5.4.3/src/lstring.c \
src_lua_interface/lua-5.4.3/src/lstrlib.c \
src_lua_interface/lua-5.4.3/src/ltable.c \
src_lua_interface/lua-5.4.3/src/ltablib.c \
src_lua_interface/lua-5.4.3/src/ltm.c \
src_lua_interface/lua-5.4.3/src/lua.c \
src_lua_interface/lua-5.4.3/src/lundump.c \
src_lua_interface/lua-5.4.3/src/lutf8lib.c \
src_lua_interface/lua-5.4.3/src/lvm.c \
src_lua_interface/lua-5.4.3/src/lzio.c


OBJS = $(SRCS:.cc=.o) $(SRCS_LUA:.c=.o)
DEPS = \
src/GenericContainer.hh \
src/GenericContainerConfig.hh \
src/GenericContainerLibs.hh \
src_json_interface/GenericContainerJson.hh \
src_json_interface/GenericContainerJsonHandler.hh \
src_lua_interface/GenericContainerLuaInterface.hh

# prefix for installation, use make PREFIX=/new/prefix install
# to override
PREFIX    = /opt/local
FRAMEWORK = GenericContainer

INC     += -I./src
LIB_DIR  = -L./lib
LIBS     = $(LIB_DIR) -lGenericContainer -lm
DEFINE   =

MKDIR  = mkdir -p

all: lib

tests: lib
	$(MKDIR) bin
	$(CXX) $(CXXFLAGS) $(INC) -o bin/example1      examples/example1.cc $(LIBS)
	$(CXX) $(CXXFLAGS) $(INC) -o bin/example2      examples/example2.cc $(LIBS)
	$(CXX) $(CXXFLAGS) $(INC) -o bin/example3      examples/example3.cc $(LIBS)
	$(CXX) $(CXXFLAGS) $(INC) -o bin/example4      examples/example4.cc $(LIBS)
	$(CXX) $(CXXFLAGS) $(INC) -o bin/example5      examples/example5.cc $(LIBS)
	$(CC)  $(CFLAGS)   $(INC) -o bin/example6      examples/example6.c  $(LIBS) $(LIBSGCC)
	$(CXX) $(CXXFLAGS) $(INC) -o bin/example7      examples/example7.cc $(LIBS)
	$(CXX) $(CXXFLAGS) $(INC) -o bin/example8      examples/example8.cc $(LIBS)
	$(CXX) $(CXXFLAGS) $(INC) -o bin/example_lua1  src_lua_interface/tests/example_lua1.cc $(LIBS)
	$(CXX) $(CXXFLAGS) $(INC) -o bin/example_lua2  src_lua_interface/tests/example_lua2.cc $(LIBS)
	$(CXX) $(CXXFLAGS) $(INC) -o bin/example_lua3  src_lua_interface/tests/example_lua3.cc $(LIBS)
	$(CXX) $(CXXFLAGS) $(INC) -o bin/JSON_test_1   src_json_interface/tests/JSON_test_1.cc $(LIBS)

lib: lib/$(LIB_GC)

src/%.o: src/%.cc $(DEPS)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

src/%.o: src/%.c $(DEPS)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

src_json_interface/%.o: src_json_interface/%.cc $(DEPS)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

src_lua_interface/%.o: src_lua_interface/%.cc $(DEPS)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

src_lua_interface/lua-5.4.3/src/%.o: src_lua_interface/lua-5.4.3/src/%.c $(DEPS)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

include_local:
	@rm -rf lib/include
	$(MKDIR) lib
	$(MKDIR) lib/include
	@cp -f src/*.hh lib/include
	@cp -f src/GenericContainerCinterface.h lib/include
	@cp -f src_json_interface/*.hh lib/include
	@cp -f src_lua_interface/*.hh lib/include

lib/libGenericContainer.a: $(OBJS) include_local
	$(MKDIR) lib
	$(AR) lib/libGenericContainer.a $(OBJS)

lib/libGenericContainer.dylib: $(OBJS) include_local
	$(MKDIR) lib
	$(CXX) -dynamiclib $(OBJS) -o lib/libGenericContainer.dylib $(LIB_DIR) -install_name libGenericContainer.dylib -Wl,-rpath,.

lib/libGenericContainer.so: $(OBJS) include_local
	$(MKDIR) lib
	$(CXX) -shared $(OBJS) -o lib/libGenericContainer.so $(LIB_DIR)

install: lib/$(LIB_GC)
	@cp -f src/*.hh $(PREFIX)/include
	@cp -f src/GenericContainerCinterface.h $(PREFIX)/include
	@cp -f src_json_interface/*.hh $(PREFIX)/include
	@cp -f src_lua_interface/*.hh $(PREFIX)/include
	cp lib/$(LIB_GC) $(PREFIX)/lib

install_as_framework: lib/$(LIB_GC)
	$(MKDIR) $(PREFIX)/include/$(FRAMEWORK)
	@cp -f src/*.hh $(PREFIX)/include/$(FRAMEWORK)
	@cp -f src/GenericContainerCinterface.h $(PREFIX)/include/$(FRAMEWORK)
	@cp -f src_json_interface/*.hh $(PREFIX)/include/$(FRAMEWORK)
	@cp -f src_lua_interface/*.hh $(PREFIX)/include/$(FRAMEWORK)
	cp lib/$(LIB_GC) $(PREFIX)/lib

run: tests
	cd bin; ./example1
	cd bin; ./example2
	cd bin; ./example3
	cd bin; ./example4
	cd bin; ./example5
	cd bin; ./example6
	cd bin; ./example7
	cd bin; ./example8
	cd src_lua_interface/tests; ../../bin/example_lua1
	cd src_lua_interface/tests; ../../bin/example_lua2
	cd src_lua_interface/tests; ../../bin/example_lua3
	cd src_json_interface/tests; ../../bin/JSON_test_1

travis: all tests run

doc:
	doxygen

clean:
	rm -f lib/libGenericContainer.* src*/*.o
	rm -f bin/*
