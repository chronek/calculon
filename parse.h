#ifndef CALCULON_PARSE_H
#include <stdio.h>
#define CALCULON_PARSE_H
#define MAX_WORD_LEN 100
#define MAX_TIME 50

typedef struct Arg {
   char value[MAX_WORD_LEN + 1];
   struct Arg *next;
} Arg;

typedef struct Test {
   char inName[MAX_WORD_LEN + 1];
   char outName[MAX_WORD_LEN + 1];
   char *time;
   int numArgs;
   Arg *args;
   struct Test *next;
} Test;

typedef struct BuildFile {
   char buildF[MAX_WORD_LEN + 1];
   struct BuildFile *next;
} BuildFile;

typedef struct Program {
   char exeName[MAX_WORD_LEN + 1];
   int numTests;
   BuildFile *builds;
   Test *tests;
   int numBuilds;
   struct Program *next;
} Program;

Test *NewTest();
BuildFile *NewBuild();
Program *NewProgram(char *prog);
Arg *NewArg();
Program *firstIter(FILE *in);
Program *DeleteProgram(Program *prog);

#endif

