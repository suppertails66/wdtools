IDIR := src
SRCDIR := src
ODIR := obj
LDIR :=

AR=ar
CXX=g++
# Compile only
CDEFINES = -DBLACKT_ENABLE_LIBPNG -DBLACKT_ENABLE_FREETYPE -DBLACKT_ENABLE_VORBISFILE -DBLACKT_ENABLE_SDL2
#CLIBS = -lSDL2 -lfreetype -lvorbisfile -lpng
CFLAGS = -std=gnu++11 -O2 -Wall
CINCLUDES = -I/usr/include/SDL2 -I/usr/local/include/SDL2 -I/usr/include/freetype2 -I/usr/include/vorbis
CXXFLAGS=$(CFLAGS) $(CDEFINES) $(CINCLUDES) -I$(IDIR)
#CXXFLAGS=-std=gnu++11 -O2 -Wall -lSDL2 -lfreetype -lvorbisfile $(DEFINES) -I$(IDIR)

SRC := $(wildcard $(SRCDIR)/*/*.cpp)
OBJ := $(patsubst $(SRCDIR)/%,$(ODIR)/%,$(patsubst %.cpp,%.o,$(SRC)))
DEP := $(patsubst %.o,%.d,$(OBJ))
LIB := libblackt.a

all: $(LIB)

-include $(DEP)

$(ODIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c -MMD -MP -MF $(@:.o=.d) -o $@ $< $(CXXFLAGS)

$(LIB): $(OBJ)
	$(AR) rcs $@ $^
	#doxygen Doxyfile

.PHONY: clean

clean:
	rm -f $(LIB)
	rm -rf $(ODIR)
