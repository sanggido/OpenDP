EXE = OpenDP
CXX = g++

OPTFLAG = -m64 -O3
LIBINCLUDE =  

CFLAGS = -std=c++0x -DNDEBUG $(OPTFLAG) $(LIBINCLUDE)

# linking flag settings
LINKCFLAGS = $(CFLAGS) -static-libstdc++ -static-libgcc

# source settings.
$(EXE): check_legal.o parser.o parser_helper.o utility.o place.o assign.o mymeasure.o main.o
	$(CXX) $^ -o $@ $(LINKCFLAGS)

check_legal.o: check_legal.cpp circuit.h
	$(CXX) $(CFLAGS) -c -o $@ $<

parser.o: parser.cpp circuit.h
	$(CXX) $(CFLAGS) -c -o $@ $<

parser_helper.o: parser_helper.cpp
	$(CXX) $(CFLAGS) -c -o $@ $<

utility.o: utility.cpp circuit.h
	$(CXX) $(CFLAGS) -c -o $@ $<

place.o: place.cpp circuit.h
	$(CXX) $(CFLAGS) -c -o $@ $<

assign.o: assign.cpp circuit.h
	$(CXX) $(CFLAGS) -c -o $@ $<

mymeasure.o: mymeasure.cpp mymeasure.h
	$(CXX) $(CFLAGS) -c -o $@ $<

main.o: main.cpp circuit.h
	$(CXX) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	/bin/rm -f $(EXE) 2> /dev/null
	/bin/rm -f *.o 2> /dev/null
