CPP = gcc

CALCULON = Calculon.o parse.o RunProgram.o SmartAlloc.o
RUNPROGRAM = RunProgram.c RunProgram.h
SMART = SmartAlloc.c SmartAlloc.h
PARSE = parse.c parse.h

calc: $(CALCULON)
	$(CPP) $(CALCULON) -o calc

%.o: %.c
	$(CPP) -c $< -o $@

Calculon.c: parse.h RunProgram.h SmartAlloc.h
	touch Calculon.c

clean:
	rm *.o
