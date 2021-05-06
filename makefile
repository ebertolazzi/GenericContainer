# get the type of OS currently running
OS      = $(shell uname)
LIB_GC  = libGenericContainer.a
CC      = gcc
CXX     = g++ -std=c++11
INC    += -I./lib3rd/include
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

SRCS = \
src/GenericContainer.cc \
src/GenericContainerSupport.cc \
src/GenericContainerTables.cc \
src/GenericContainerCinterface.cc

OBJS = $(SRCS:.cc=.o)
DEPS = \
src/GenericContainer.hh \
src/GenericContainerCinterface.h

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
	$(CXX) $(CXXFLAGS) $(INC) -o bin/example1  examples/example1.cc  $(LIBS)
	$(CXX) $(CXXFLAGS) $(INC) -o bin/example2  examples/example2.cc  $(LIBS)
	$(CXX) $(CXXFLAGS) $(INC) -o bin/example3  examples/example3.cc  $(LIBS)
	$(CXX) $(CXXFLAGS) $(INC) -o bin/example4  examples/example4.cc  $(LIBS)
	$(CXX) $(CXXFLAGS) $(INC) -o bin/example5  examples/example5.cc  $(LIBS)
	$(CC)  $(CFLAGS)   $(INC) -o bin/example6  examples/example6.c   $(LIBS) $(LIBSGCC)
	$(CXX) $(CXXFLAGS) $(INC) -o bin/example7  examples/example7.cc  $(LIBS)
	$(CXX) $(CXXFLAGS) $(INC) -o bin/example8  examples/example8.cc  $(LIBS)

lib: lib/$(LIB_GC)

src/%.o: src/%.cc $(DEPS)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

src/%.o: src/%.c $(DEPS)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

include_local:
	@rm -rf lib/include
	$(MKDIR) lib
	$(MKDIR) lib/include
	@cp -f src/GenericContainer.hh lib/include
	@cp -f src/GenericContainerConfig.hh lib/include
	@cp -f src/GenericContainerCinterface.h lib/include

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
	cp lib/include/GenericContainer.hh $(PREFIX)/include
	cp lib/include/GenericContainerConfig.hh $(PREFIX)/include
	cp lib/include/GenericContainerCinterface.h $(PREFIX)/include
	cp lib/$(LIB_GC) $(PREFIX)/lib

install_as_framework: lib/$(LIB_GC)
	$(MKDIR) $(PREFIX)/include/$(FRAMEWORK)
	cp lib/include/GenericContainerConfig.hh $(PREFIX)/include/$(FRAMEWORK)
	cp lib/include/GenericContainerCinterface.h $(PREFIX)/include/$(FRAMEWORK)
	cp lib/include/GenericContainer.hh $(PREFIX)/include/$(FRAMEWORK)
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

travis: all tests run

doc:
	doxygen

clean:
	rm -f lib/libGenericContainer.* src*/*.o
	rm -f bin/example1
	rm -f bin/example2
	rm -f bin/example3
	rm -f bin/example4
	rm -f bin/example5
	rm -f bin/example6
	rm -f bin/example7
	rm -f bin/example8
	rm -f bin/example9
