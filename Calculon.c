#include <stdio.h>
#include "parse.h"
#include "RunProgram.h"
#include <string.h>
#ifndef MAX_WORD_LEN
#define MAX_WORD_LEN 100
#endif

// reads in the programs to test and all of
// the associated data
static Program *readProgs(Program *prog, FILE *in) {
   int nextChar;
   char nextWord[MAX_WORD_LEN + 1], *time = 0;
   Program *rtn, *lastProg;
   Test *lastTest;
   Arg *lastArg;
   BuildFile *lastBuild;

   rtn = lastProg = prog;
   lastTest = prog->tests;
   do {
      // make a program and set values
      if ((nextChar = getc(in)) == 'P') {
         fscanf(in, "%s", nextWord);
         lastProg = lastProg->next = NewProgram(nextWord);
         lastTest = lastProg->tests;
         time = 0;

         if ((nextChar = getc(in)) != '\n') {
            lastBuild = lastProg->builds = NewBuild();

            // loop until an end line or eof
            while ((nextChar = getc(in)) != '\n' && nextChar != EOF) {
               ungetc(nextChar, in);
               fscanf(in, "%s", lastBuild->buildF);
               lastProg->numBuilds++;

               // if there is another build to make on the line
               if ((nextChar = getc(in)) != '\n' && nextChar != EOF)
                  lastBuild = lastBuild->next = NewBuild();

               ungetc(nextChar, in);
            }
         }
         // make a test for the last program and set values
      } else if (nextChar == 'T') {
         lastProg->numTests++;

         fscanf(in, "%s", lastTest->inName);

         fscanf(in, "%s", lastTest->outName);

         fscanf(in, "%50s", nextWord);
         strncpy(lastTest->time, nextWord, 50);

         if ((nextChar = getc(in)) != '\n') {
            lastArg = lastTest->args = NewArg();

            // loop until an end line or eof
            while ((nextChar = getc(in)) != '\n' && nextChar != EOF) {
               ungetc(nextChar, in);

               fscanf(in, "%s", lastArg->value);

               // make a new arg if there is another one on the line
               if ((nextChar = getc(in)) != '\n' && nextChar != EOF) {
                  lastArg = lastArg->next = NewArg();
                  lastTest->numArgs++;
               }
               ungetc(nextChar, in);
            }
         }

         // check if another test is needed before a new program is made
         if ((nextChar = getc(in)) == 'T') {
            ungetc(nextChar, in);

            // make a new test if there is another test for lastProg
            if ((nextChar = getc(in)) != '\n' && nextChar != EOF) {
               ungetc(nextChar, in);
               lastTest = lastTest->next = NewTest();
            }
         } else
            ungetc(nextChar, in);
      }
   }while (nextChar != EOF);

   return rtn;
}

// run a program with SafeRun and report the exit code
int main(int argc, char **argv) {
   Program *progs, *curProg;
   FILE *in;

   in = fopen(argv[1], "r");

   progs = curProg = readProgs(firstIter(in), in);
   fclose(in);

   //make all the programs and run the test
   for (; curProg; curProg = curProg->next) {
      if (callMake(curProg))
         runProgram(curProg);
   }

   // free all of the allocated space
   while (progs != NULL)
     progs = DeleteProgram(progs);

   return 0;
}
