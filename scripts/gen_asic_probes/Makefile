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
exec = $(bin)/gen-asic-probes.exe
execdbg = $(bin)/gen-asic-probes-dbg.exe


all : config

config : $(srcfls)
	mkdir -p build/
	$(CXX) $(srcfls) -I $(src) $(CXXFLAGS) $(OPTIMFLAGS) $(LDFLAGS) -o $(exec)
	cp $(exec) ../

debug : $(srcfls)
	$(CXX) $(srcfls) -I $(src) $(CXXFLAGS) $(DBGFLAGS) $(LDFLAGS) -o $(execdbg)

.PHONY : clean

clean :
	rm -f $(src)/*.o $(bin)/*.o 
	rm -r build/

