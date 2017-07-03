IDIR := src
SRCDIR := src
ODIR := obj
LDIR :=

AR=ar
CXX=g++
CDEFINES = -DBLACKT_ENABLE_LIBPNG -DBLACKT_ENABLE_FREETYPE -DBLACKT_ENABLE_VORBISFILE -DBLACKT_ENABLE_SDL2
CLIBS = -L./blackt -lblackt -lSDL2 -lvorbisfile -lpng
CFLAGS = -std=gnu++11 -O2 -Wall
CINCLUDES = -I/usr/include/SDL2 -I/usr/local/include/SDL2 -I/usr/include/freetype2 -I/usr/include/vorbis -I./blackt/src
CXXFLAGS=$(CFLAGS) $(CDEFINES) $(CINCLUDES) -I$(IDIR) $(CLIBS)
#CXXFLAGS=-std=gnu++11 -O2 -Wall -lSDL2 -lfreetype -lvorbisfile $(DEFINES) -I$(IDIR)

SRC := $(wildcard $(SRCDIR)/*/*.cpp)
OBJ := $(patsubst $(SRCDIR)/%,$(ODIR)/%,$(patsubst %.cpp,%.o,$(SRC)))
DEP := $(patsubst %.o,%.d,$(OBJ))
#LIB := libblackt.a
LIB := libcopycat.a

PREFIX := /usr/local
BINDIR := $(PREFIX)/bin
INSTALL := install

tools = scriptrip scriptrip_jp tablerip strinsert pngcompare itemmaker blockcopy relsearch popencmp popshcmp vaycmp vaydecmp vayitems vaymonsters hucdism exilestats chngsrch ex2enemy alundat scriptrip_tss strinsert_tss stats_tss items_tss mapcp_tss lunardat mapcp_eb sampcm aluncmp mkr_enemies gearbolt_decmp lsss_extr gearbolt_cmp lsss_stats lsss_money lsss_txt l2eb_data l2eb_txt l2eb_grp l2eb_stats cf2_stats

all: blackt copycat $(OBJ) $(tools)
	
copycat: blackt $(OBJ)
	$(CXX) $(OBJ) src/main.cpp -o grp $(CXXFLAGS)
	
scriptrip: blackt $(OBJ)
	$(CXX) $(OBJ) src/scriptrip.cpp -o scriptrip $(CXXFLAGS)
	
scriptrip_jp: blackt $(OBJ)
	$(CXX) $(OBJ) src/scriptrip_jp.cpp -o scriptrip_jp $(CXXFLAGS)
	
tablerip: blackt $(OBJ)
	$(CXX) $(OBJ) src/tablerip.cpp -o tablerip $(CXXFLAGS)
	
strinsert: blackt $(OBJ)
	$(CXX) $(OBJ) src/strinsert.cpp -o strinsert $(CXXFLAGS)
	
pngcompare: blackt $(OBJ)
	$(CXX) $(OBJ) src/pngcompare.cpp -o pngcompare $(CXXFLAGS)
	
itemmaker: blackt $(OBJ)
	$(CXX) $(OBJ) src/itemmaker.cpp -o itemmaker $(CXXFLAGS)
	
blockcopy: blackt $(OBJ)
	$(CXX) $(OBJ) src/blockcopy.cpp -o blockcopy $(CXXFLAGS)
	
relsearch: blackt $(OBJ)
	$(CXX) $(OBJ) src/relsearch.cpp -o relsearch $(CXXFLAGS)
	
popencmp: blackt $(OBJ)
	$(CXX) $(OBJ) src/popencmp.cpp -o popencmp $(CXXFLAGS)
	
popshcmp: blackt $(OBJ)
	$(CXX) $(OBJ) src/popshcmp.cpp -o popshcmp $(CXXFLAGS)
	
vaycmp: blackt $(OBJ)
	$(CXX) $(OBJ) src/vaycmp.cpp -o vaycmp $(CXXFLAGS)
	
vaydecmp: blackt $(OBJ)
	$(CXX) $(OBJ) src/vaydecmp.cpp -o vaydecmp $(CXXFLAGS)
	
vayitems: blackt $(OBJ)
	$(CXX) $(OBJ) src/vayitems.cpp -o vayitems $(CXXFLAGS)
	
vaymonsters: blackt $(OBJ)
	$(CXX) $(OBJ) src/vaymonsters.cpp -o vaymonsters $(CXXFLAGS)
	
hucdism: blackt $(OBJ)
	$(CXX) $(OBJ) src/hucdism.cpp -o hucdism $(CXXFLAGS)
	
exilestats: blackt $(OBJ)
	$(CXX) $(OBJ) src/exilestats.cpp -o exilestats $(CXXFLAGS)
	
chngsrch: blackt $(OBJ)
	$(CXX) $(OBJ) src/chngsrch.cpp -o chngsrch $(CXXFLAGS)
	
ex2enemy: blackt $(OBJ)
	$(CXX) $(OBJ) src/ex2enemy.cpp -o ex2enemy $(CXXFLAGS)
	
alundat: blackt $(OBJ)
	$(CXX) $(OBJ) src/alundat.cpp -o alundat $(CXXFLAGS)
	
scriptrip_tss: blackt $(OBJ)
	$(CXX) $(OBJ) src/scriptrip_tss_utils.cpp src/scriptrip_tss.cpp -o scriptrip_tss $(CXXFLAGS)
	
strinsert_tss: blackt $(OBJ)
	$(CXX) $(OBJ) src/strinsert_tss.cpp -o strinsert_tss $(CXXFLAGS)
	
stats_tss: blackt $(OBJ)
	$(CXX) $(OBJ) src/scriptrip_tss_utils.cpp src/stats_tss.cpp -o stats_tss $(CXXFLAGS)
	
items_tss: blackt $(OBJ)
	$(CXX) $(OBJ) src/scriptrip_tss_utils.cpp src/items_tss.cpp -o items_tss $(CXXFLAGS)
	
mapcp_tss: blackt $(OBJ)
	$(CXX) $(OBJ) src/scriptrip_tss_utils.cpp src/mapcp_tss.cpp -o mapcp_tss $(CXXFLAGS)
	
lunardat: blackt $(OBJ)
	$(CXX) $(OBJ) src/lunardat.cpp -o lunardat $(CXXFLAGS)
	
mapcp_eb: blackt $(OBJ)
	$(CXX) $(OBJ) src/mapcp_eb.cpp -o mapcp_eb $(CXXFLAGS)
	
sampcm: blackt $(OBJ)
	$(CXX) $(OBJ) src/sampcm.cpp -o sampcm $(CXXFLAGS)
	
aluncmp: blackt $(OBJ)
	$(CXX) $(OBJ) src/aluncmp.cpp -o aluncmp $(CXXFLAGS)
	
mkr_enemies: blackt $(OBJ)
	$(CXX) $(OBJ) src/mkr_enemies.cpp -o mkr_enemies $(CXXFLAGS)
	
gearbolt_decmp: blackt $(OBJ)
	$(CXX) $(OBJ) src/gearbolt_decmp.cpp -o gearbolt_decmp $(CXXFLAGS)
	
lsss_extr: blackt $(OBJ)
	$(CXX) $(OBJ) src/lsss_extr.cpp -o lsss_extr $(CXXFLAGS)
	
gearbolt_cmp: blackt $(OBJ)
	$(CXX) $(OBJ) src/gearbolt_cmp.cpp -o gearbolt_cmp $(CXXFLAGS)
	
lsss_stats: blackt $(OBJ)
	$(CXX) $(OBJ) src/lsss_stats.cpp -o lsss_stats $(CXXFLAGS)
	
lsss_money: blackt $(OBJ)
	$(CXX) $(OBJ) src/lsss_money.cpp -o lsss_money $(CXXFLAGS)
	
lsss_txt: blackt $(OBJ)
	$(CXX) $(OBJ) src/lsss_txt.cpp -o lsss_txt $(CXXFLAGS)
	
l2eb_data: blackt $(OBJ)
	$(CXX) $(OBJ) src/l2eb_data.cpp -o l2eb_data $(CXXFLAGS)
	
l2eb_txt: blackt $(OBJ)
	$(CXX) $(OBJ) src/l2eb_txt.cpp -o l2eb_txt $(CXXFLAGS)
	
l2eb_grp: blackt $(OBJ)
	$(CXX) $(OBJ) src/l2eb_grp.cpp -o l2eb_grp $(CXXFLAGS)
	
l2eb_stats: blackt $(OBJ)
	$(CXX) $(OBJ) src/l2eb_stats.cpp -o l2eb_stats $(CXXFLAGS)
	
cf2_stats: blackt $(OBJ)
	$(CXX) $(OBJ) src/cf2_stats.cpp -o cf2_stats $(CXXFLAGS)
	
#libcopycat: $(OBJ)
#	$(AR) rcs $(LIB) $^

-include $(DEP)

$(ODIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c -MMD -MP -MF $(@:.o=.d) -o $@ $< $(CXXFLAGS)

blackt: blackt/libblackt.a

blackt/libblackt.a:
	cd ./blackt && $(MAKE) && cd $(CURDIR)

#$(LIB): $(OBJ)
#	$(AR) rcs $@ $^
	#doxygen Doxyfile

.PHONY: clean install

clean:
#	rm -f $(LIB)
	cd ./blackt && $(MAKE) clean && cd $(CURDIR)
	rm -rf $(ODIR)
	rm -f $(tools)
	rm -f grp

install: all
	$(INSTALL) -d $(BINDIR)
	$(INSTALL) $(tools) $(BINDIR)
