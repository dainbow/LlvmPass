CC = clang
LLC = llc
OPT = opt
DOT = dot

PASSDIR = ~/Git/llvm-pass/pass/
PASSNAME = libpass.so

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

SOURCES = $(wildcard $(SRCDIRS)*.c)
TARGETS = $(wildcard $(TARGETDIRS)*.c)
GRAPHS = $(wildcard $(GRAPHDIR)*.txt)

OBJECTS = $(addprefix $(BINDIR), $(notdir $(SOURCES:.c=.o)))
TARGETOBJECTS = $(addprefix $(BINDIR), $(notdir $(TARGETS:.c=.o)))
DUMPS = $(addprefix $(DUMPDIR), $(notdir $(GRAPHS:.txt=.png)))

DEPENDENCES = $(addsuffix .d,$(OBJECTS))
TARGETDEPENDENCES = $(addsuffix .d,$(TARGETOBJECTS))

TARGETSNOEXT =  $(notdir $(TARGETS:.c=))
DUMPSNOEXT = $(notdir $(GRAPHS:.txt=))

$(TARGETSNOEXT): $(OBJECTS) $(TARGETOBJECTS)
	@$(CC) $(LDFLAGS) $(OBJECTS) $(BINDIR)$@.o -o $@.out

$(DUMPDIR)%.png: $(GRAPHDIR)%.txt
	@$(DOT) $(DOTFLAGS) $< > $@

$(BINDIR)%.o: $(SRCDIRS)%.c $(PASSDIR)$(PASSNAME)
	@$(CC) -MMD -MF $@.d $(CXXFLAGS) $< -o $@.bc
	@$(OPT) $(OPTFLAGS) -load $(PASSDIR)$(PASSNAME) -mypass $@.bc -o $@.bc
	@$(CC) $@.bc -c -o $@
 
$(BINDIR)%.o: $(TARGETDIRS)%.c $(PASSDIR)$(PASSNAME)
	@$(CC) -MMD -MF $@.d $(CXXFLAGS) $< -o $@.bc
	@$(OPT) $(OPTFLAGS) -load $(PASSDIR)$(PASSNAME) -mypass $@.bc -o $@.bc
	@$(CC) $@.bc -c -o $@

$(PASSDIR)$(PASSNAME): $(PASSDIR)MyPass.cpp
	@make -C $(PASSDIR)

-include $(DEPENDENCES)
-include $(TARGETDEPENDENCES)

.PHONY: all, clean, mkdirs, dumps

all: Library

dumps: $(DUMPS)
	@echo "Dumps created"

mkdirs:
	mkdir $(BINDIR) $(SRCDIRS) $(TARGETDIRS) $(INCLUDEDIR)

clean: 
	rm $(OBJECTS) $(TARGETOBJECTS) $(DEPENDENCES) $(TARGETDEPENDENCES) $(PASSDIR)$(PASSNAME)
