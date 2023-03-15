CC = clang++
LLC = llc
OPT = opt
DOT = dot

PASSDIR = ./pass/spectre/
PASSNAME = libSpectre.so

CXXFLAGS  = -c -O0 -emit-llvm -Wall -Wextra
LDFLAGS = 
OPTFLAGS = -enable-new-pm=0
DOTFLAGS = -Tpng

SRCDIRS = ./src/
TARGETDIRS = ./targets/
BINDIR = ./bin/
INCLUDEDIR = ./include/
GRAPHDIR = ./graphs/
DUMPDIR = ./dumps/
DATADIR = ./data/

SOURCES = $(wildcard $(SRCDIRS)*.cpp)
TARGETS = $(wildcard $(TARGETDIRS)*.cpp)
GRAPHS = $(wildcard $(GRAPHDIR)*.txt)

OBJECTS = $(addprefix $(BINDIR), $(notdir $(SOURCES:.cpp=.o)))
TARGETOBJECTS = $(addprefix $(BINDIR), $(notdir $(TARGETS:.cpp=.o)))
DUMPS = $(addprefix $(DUMPDIR), $(notdir $(GRAPHS:.txt=.png)))

DEPENDENCES = $(addsuffix .d,$(OBJECTS))
TARGETDEPENDENCES = $(addsuffix .d,$(TARGETOBJECTS))

TARGETSNOEXT =  $(notdir $(TARGETS:.cpp=))
DUMPSNOEXT = $(notdir $(GRAPHS:.txt=))

$(TARGETSNOEXT): $(OBJECTS) $(TARGETOBJECTS)
	@$(CC) $(LDFLAGS) $(OBJECTS) $(BINDIR)$@.o -o $@.out

$(DUMPDIR)%.png: $(GRAPHDIR)%.txt
	@$(DOT) $(DOTFLAGS) $< > $@

$(BINDIR)%.o: $(SRCDIRS)%.cpp $(PASSDIR)$(PASSNAME)
	@$(CC) -MMD -MF $@.d $(CXXFLAGS) $< -o $@.bc
	@$(OPT) $(OPTFLAGS) -load $(PASSDIR)$(PASSNAME) -spectre $@.bc -o $@.bc
	@$(CC) $@.bc -c -o $@
 
$(BINDIR)%.o: $(TARGETDIRS)%.cpp $(PASSDIR)$(PASSNAME)
	@$(CC) -MMD -MF $@.d $(CXXFLAGS) $< -o $@.bc
	@$(OPT) $(OPTFLAGS) -load $(PASSDIR)$(PASSNAME) -spectre $@.bc -o $@.bc
	@$(CC) $@.bc -c -o $@

$(PASSDIR)$(PASSNAME): $(PASSDIR)spectre.cpp
	@make -C $(PASSDIR)

-include $(DEPENDENCES)
-include $(TARGETDEPENDENCES)

.PHONY: all, clean, mkdirs, dumps

all: Library

init:
	cmake -S ./pass/ -B ./pass/
	cmake -S ./pass/spectre/ -B ./pass/spectre/
	make mkdirs

dumps: $(DUMPS)
	@echo "Dumps created"

mkdirs:
	mkdir $(BINDIR) $(GRAPHDIR) $(DUMPDIR) $(DATADIR)

clean: 
	@rm -f $(BINDIR)*
	@rm -f $(GRAPHDIR)*
	@rm -f $(DUMPDIR)*
	@rm -f $(DATADIR)*
