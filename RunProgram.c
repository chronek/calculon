#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "SmartAlloc.h"
#include <sys/wait.h>
#include "RunProgram.h"

struct SafeRunErrors {
   int diffErr;
   int runTime;
   int error;
};

int callMake(Program *prog) {
   int status, exitVal;

   if (prog != NULL) {
      // fork a process
      int pid = fork();

      // exec make with the child process
      // otherwise, wait for the child to finish

      if (pid == 0) {
         freopen("/dev/null", "w", stdout);
         freopen("/dev/null", "w", stderr);

         execl("/usr/bin/make", "make", prog->exeName, NULL);
         exit(0);
      } else
         wait(&status);
      exitVal = WEXITSTATUS(status);
      if (exitVal != 0) {
         if ((pid = fork()) == 0) {
            freopen("/dev/null", "w", stdout);
            //freopen("/dev/null", "w", stderr);
 
            char **args = gccBuild(prog);
            execvp("gcc", args);
            exit(0);
         } else {
            wait(&status);

            if (WEXITSTATUS(status) != 0) {
               printf("Failed: ");
               char **gcc = gccBuild(prog);
               while (*gcc) 
	          printf("%s ", *gcc++);
               printf("\n");
               return 0;
            }
            return 1;
         }
      }
     return 1;
   }
}


void runProgram(Program *prog) {
   SafeRunErrors *errors;
   int pid, pass = 0;
   int testNum = 1, status, diffEr = 0;

   //fork and exec each test run of the program
   for (Test *curTest = prog->tests; curTest; curTest = curTest->next) {
      // run the program with SafeRun
      if ((pid = fork()) == 0) {
         freopen("test.output.temp", "w", stdout);
         freopen("test.output.temp", "w", stderr);

         freopen(curTest->inName, "r", stdin);

         execvp("SafeRun", makeCmdArgs(prog));
         exit(0);
      } else
         wait(&status);
      // save off exit status of run
      errors = evalExitCode(WEXITSTATUS(status));

      if ((pid = fork()) == 0) {
         freopen("/dev/null", "w", stdout);
         freopen("/dev/null", "w", stderr);

         execvp("diff", diffArg(prog));
         exit(0);
      } else {
         wait(&status);
      errors->diffErr = WEXITSTATUS(status);
      }
      // check for runtime errors in the program
      //if (errors->diffErr || errors->runTime || errors->error)
        // printErrors(errors, prog, testNum++);
      if (errors->diffErr == 0)
         pass++;
      remove("test.output.temp");
   }
   if (pass == prog->numTests)
      printf("%s %d of %d tests passed.\n", prog->exeName, pass,
       prog->numTests);
}


char **diffArg(Program *prog) {
   char **arg, **thisArg = calloc(4, sizeof(char *));
   *thisArg++ = "diff";
   *thisArg++ = "test.output.temp";
   *thisArg++ = prog->tests->outName;
   *thisArg++ = NULL;

   return arg;
}


char **makeCmdArgs(Program *prog) {
   char **argList, **thisArg;
   Test *test;
   // fill a list of args that this program has
   argList = thisArg = calloc(5 + prog->tests->numArgs, sizeof(char *));

   test = prog->tests;
   *thisArg++ = "SafeRun";
   *thisArg++ = "-p30";
   //allocate space for the time arg and copy the contents in
   char *timeArg = calloc(1, strlen("-t") + strlen(test->time));
   strcat(timeArg, "-t");
   strcat(timeArg, test->time);
   *thisArg++ = timeArg;

   char *progArg = calloc(strlen("./") + strlen(prog->exeName), 1);
   strcat(progArg, "./");
   strcat(progArg, prog->exeName);
   *thisArg++ = progArg;

   // add args we want for this test to argv list
   while (test->args != NULL) {
      *thisArg++ = test->args->value;
      test->args = test->args->next;
   }

   return argList;
}


SafeRunErrors *evalExitCode(int exit) {
   SafeRunErrors *s1 = calloc(1, sizeof(SafeRunErrors));
   // build out the struct that contains what errors there were
   //printf("%d exit code\n", exit);
   if (exit > 192) {
      exit %= 192;
      if (exit >= 8) {
         s1->error = 1;
         exit -= 8;
      }

      if (exit >= 2) {
         s1->runTime = 1;
         exit -= 2;
      }

      if (exit % 2 != 0)
         s1->runTime = 1;
      return s1;
   }
}


char **gccBuild(Program *prog) {
   char **builds, **thisBuild;
   builds = thisBuild = calloc(prog->numBuilds + 4, sizeof(char *));
   BuildFile *curBuild = prog->builds;
   
   *thisBuild++ = "gcc";
   while (curBuild != NULL) {
      *thisBuild++ = curBuild->buildF;
      curBuild = curBuild->next;
   }
   *thisBuild++ = "-o";
   *thisBuild = prog->exeName;

   return builds;
}


void printErrors(SafeRunErrors *errors, Program *prog, int testNum) {
   printf("%s test %d failed: ", prog->exeName, testNum);
   if (errors->diffErr) {
      printf("diff failure");
      if (errors->runTime || errors->error)
         printf(", ");
   }

   if (errors->error) {
      printf("runtime error");
      if (errors->runTime)
         printf(", ");
   }
  
   if (errors->runTime)
      printf("timeout");
   printf("\n");
}
