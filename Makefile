NAME=fishdish
PROGRAM=main
ARCHIVE=lib$(NAME).a
LIBS=-lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

INCLUDE=include
SRC=src
TEST=test
BUILD=build
BIN=bin
LIB=lib
DOC=share/doc
# LOG=var/log
LOG=log

SOURCES=$(wildcard $(SRC)/*.cpp)
MAINS=$(PROGRAM:%=$(BUILD)/%.o)
OBJECTS=$(filter-out $(MAINS), $(SOURCES:$(SRC)/%.cpp=$(BUILD)/%.o))

vpath %.a       $(LIB)
vpath %.c       $(SRC)
vpath %.cpp     $(SRC)
vpath %.gcda    $(BUILD)
vpath %.gcno    $(BUILD)
vpath %.h       $(INCLUDE)
vpath %.hpp     $(INCLUDE)
vpath %.o       $(BUILD)
vpath %.sh      scripts

AR=ar
ARFLAGS=rv
CC=gcc
# C_INCLUDE_PATH=$(CPATH)
CFLAGS=-std=c++17 -g3
# CFLAGS=-std=c++17 -pedantic -O2 -Wall -W -Wextra -Werror
# CFLAGS=-std=gnu++17 -O2 -Wall -W -Wextra -Werror
# CFLAGS+=-g3 -pg --coverage -O0 -v # -fbranch-probabilities
# CFLAGS+=`sdl2-config --cflags`
CXX=g++
# CPLUS_INCLUDE_PATH=$(CPATH)
CXXFLAGS=$(CFLAGS)
# LDLIBS=`sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer
LDLIBS=$(LIBS)

.PHONY: all init clean
# .PHONY: all install clean distclean TAGS TAGS.gtags TAGS.cscope info check
# .PHONY: init release check-syntax check-style indent pretty

all: init $(PROGRAM)

init: $(BUILD) $(BIN) $(LIB) $(LOG)

$(BUILD) $(BIN) $(LIB) $(LOG):
	mkdir $(@)

$(MAINS) $(OBJECTS): $(BUILD)/%.o: $(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $(@) $(<)

$(PROGRAM): %: $(OBJECTS) $(BUILD)/%.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(@:%=$(BIN)/%) $(LDLIBS) $(^)

clean:
	rm -rf $(BUILD) $(BIN) $(LIB) $(LOG)
