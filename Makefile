################################################################################
# ConfigFile build makefile
#
# Author:   J. Parziale
# Created:  12/7/2010
# Modified: 10/21/2020  Port from Xerces to TinyXML2
#
################################################################################

# GCC versions under 6.0 can only use c++ std "c++0x".
# Newer ones can use c++17, c++2a, c++2b, or c++20
# C++20 - avail since GCC 8, still experimental. Use c++2a in GCC 9 and earlier.
# C++17 - avail since GCC 5, default in GCC 11
# C++14 - avail since GCC 6.1, default in GCC 10
# C++11 - avail since GCC 4.8.1, using C++0x or C++11 as follows:
#         GCC 4.3 thru 4.6 use c++0x
#         GCC 4.7 thru 4.8 use c++11
GCC_VER=$(shell $(CXX) -dumpversion | cut -f1-2 -d. | sed 's/\.//g')
MIN_GCC_VER = 60

# gcc 10.x just shows up as 10 instead of 10x
ifeq ($(GCC_VER),10)
  GCC_VER=100
endif

GCCVERSIONGT60 := $(shell expr $(GCC_VER) \>= $(MIN_GCC_VER))
OSTYPE := $(shell uname)
ifeq "$(GCCVERSIONGT60)" "1"
#    GCC_STD=-std=c++17
    GCC_STD=-std=c++20
else
    GCC_STD=-std=c++0x
endif

COMPILER = g++
CXX      = g++
CC       = g++
LINK     = g++
INCLUDES = -I./include -I../Tools/include -I../tinyxml2
#THREADS  = -lpthread -lrt

LIBDIRS = -L../tinyxml2
LIBS = $(THREADS) -ltinyxml2
LDFLAGS  =  $(LIBDIRS) $(LIBS)
LIB_FILE = bin/libconfigfile.a

CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 $(GCC_STD) $(INCLUDES)

################################################################################

OBJS = ConfigFile.o \
	../Tools/build/CMakeFiles/cpp_tools.dir/source/common.cpp.o \
	../Tools/build/CMakeFiles/cpp_tools.dir/source/trim.cpp.o \
	../tinyxml2/tinyxml2.o

#HEADER_FILES=parser.hpp

TARGETS= \
	bin/tConfigFile \
	bin/TestReadConfigData #\
#	parser 

################################################################################

#.cpp:.o
#	$(CXX) $(CXXFLAGS) $< -o $@ -c

################################################################################

all: $(TARGETS)

bin/tConfigFile: test/tConfigFile.cpp ConfigFile.o $(OBJS)
	${CC} ${CXXFLAGS} ${LDFLAGS}  -o bin/tConfigFile test/tConfigFile.cpp $(OBJS)
	@echo

bin/TestReadConfigData: test/TestReadConfigData.cpp $(OBJS)
	${CC} ${CXXFLAGS} ${LDFLAGS}  -o bin/TestReadConfigData test/TestReadConfigData.cpp $(OBJS)
	@echo

bin/parser: parser.cpp parser.hpp
#	${CC} ${CXXFLAGS} ${LDFLAGS} -o parser -DMAIN_TEST parser.cpp
	@echo "--- parser ---"
	@echo "NOTE: parser hasn't been converted from Xerces yet"
	@echo

#ConfigFile: ConfigFile.cpp ${HEADER_FILES}  $(OBJS)
#	${CC} ${CXXFLAGS} ${LDFLAGS} -o ConfigFile -DMAIN_TEST ConfigFile.cpp $(OBJS)
#	@echo

ConfigFile.o: source/ConfigFile.cpp
	${CC} ${CXXFLAGS} ${LDFLAGS} -c source/ConfigFile.cpp

################################################################################

tidy:
	$(RM) -v *.d *.map *.log
	$(RM) -r bin/*.dSYM

clean: tidy
	rm -f *.o $(TARGETS)

clobber: clean
	$(RM) -vf $(TARGETS) *.log _sample.xml
	$(RM) -vf $(LIB_FILE)
	@echo

echo:
	@echo
	@echo "OSTYPE    = $(OSTYPE)"
	@echo "IS_DARWIN = $(IS_DARWIN)"
	@echo "GCC_VER   = $(GCC_VER)"
	@echo "GCCVERSIONGT60 = $(GCCVERSIONGT60)"
	@echo "INCLUDES  = $(INCLUDES)"
	@echo "LIBDIRS   = $(LIBDIRS)"
	@echo "LIBS      = $(LIBS)"
	@echo "LDFLAGS   = $(LDFLAGS)"
	@echo "CXXFLAGS  = $(CXXFLAGS)"
	@echo "OBJS      = $(OBJS)"
	@echo "TARGETS   = $(TARGETS)"
	@echo

################################################################################
