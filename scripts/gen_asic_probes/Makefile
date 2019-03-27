CXX=g++
LDFLAGS += -lstdc++ -std=c++11
CXXFLAGS += -Wall
DBGFLAGS = -g -Og
OPTIMFLAGS = -O3 -funroll-loops 


src = ${PWD}/src
bin = ${PWD}/build

main = $(src)/main
menu = $(src)/menu
asicprobe = $(src)/asicprobe
srcfls = $(main).cpp $(menu).cpp $(asicprobe).cpp
exec = $(bin)/gen_asic_probe
execdbg = $(bin)/gen_asic_probe_dbg


all : config

config : $(srcfls)
	$(CXX) $(srcfls) -I $(src) $(CXXFLAGS) $(OPTIMFLAGS) $(LDFLAGS) -o $(exec)

debug : $(srcfls)
	$(CXX) $(srcfls) -I $(src) $(CXXFLAGS) $(DBGFLAGS) $(LDFLAGS) -o $(execdbg)

.PHONY : clean

clean :
	rm -f $(src)/*.o $(bin)/*.o 
	rm -f $(exec)
	rm -f $(execdbg)
