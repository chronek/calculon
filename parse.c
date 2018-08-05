#ifndef CALCULON_PARSE_H

#include "parse.h"
#include "SmartAlloc.h"
#include <string.h>
#include <stdio.h>


/* max length of programs and tests */
#define MAX_WORD_LEN 100

// creates a new test in the linked list
// everything is set to 0 or null
Test *NewTest() {
   Test *rtn = malloc(sizeof(Test));

   rtn->inName[0] = '\0';
   rtn->outName[0] = '\0';
   rtn->time = malloc(MAX_TIME);
   rtn->numArgs = 0;
   rtn->args = NULL;
   rtn->next = NULL;

   return rtn;
}

// creates a new buildFile with <name> and next set to null
BuildFile *NewBuild() {
   BuildFile *rtn = malloc(sizeof(BuildFile));

   rtn->buildF[0] = '\0';
   rtn->next = NULL;

   return rtn;
}

// make a new Program with name (prog)
Program *NewProgram(char *prog) {
   Program *rtn = malloc(sizeof(Program));

   strncpy(rtn->exeName, prog, MAX_WORD_LEN);
   rtn->numTests = 0;
   rtn->numBuilds = 0;
   rtn->tests = NewTest();
   rtn->builds = NULL;
   rtn->next = NULL;

   return rtn;
}

// creates a new arg with value <name> and next set to null
Arg *NewArg() {
   Arg *rtn = malloc(sizeof(Arg));

   rtn->value[0] = '\0';
   rtn->next = NULL;

   return rtn;
}

Program *firstIter(FILE *in) {
   int nextChar;
   char nextWord[MAX_WORD_LEN + 1], *time = 0;
   Program *rtn, *lastProg;
   Test *lastTest;
   Arg *lastArg;
   BuildFile *lastBuild;

   if ((nextChar = getc(in)) == 'P') {
      fscanf(in, "%s", nextWord);
      rtn = lastProg = NewProgram(nextWord);
      lastTest = lastProg->tests;
      lastBuild = lastProg->builds = NewBuild();

      while ((nextChar = getc(in)) != '\n' ) {  // check for newline character
         ungetc(nextChar, in);
         fscanf(in, "%s", lastBuild->buildF);
         lastProg->numBuilds++;

         if ((nextChar = getc(in)) != '\n')
            lastBuild = lastBuild->next = NewBuild();

         ungetc(nextChar, in);
      }
      return lastProg;
   } else
      return NULL;
}
// frees the allocated memory for Programs
Program *DeleteProgram(Program *prog) {
   Test *temp;
   BuildFile *bf;
   Arg *tempArg;
   Program *tempProg;

   // free all of the builf file structs
   while (prog->builds != NULL) {
      bf = prog->builds;
      prog->builds = bf->next;
      free(bf);
   }

   // free all of the tests
   while (prog->tests != NULL) {
      temp = prog->tests;
      prog->tests = temp->next;

      // in each test, free all of the args
      while (temp->args != NULL) {
         tempArg = temp->args;

         temp->args = tempArg->next;
         free(tempArg);

      }
      // free the test
      free(temp->time);
      free(temp);
   }

   // free the current program
   tempProg = prog;
   prog = prog->next;
   free(tempProg);

   return prog;
}

#endif
