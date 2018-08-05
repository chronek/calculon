#ifndef CALCULON_RUNPROGRAM_H
#define CALCULON_RUNPROGRAM_H
#include "parse.h"

typedef struct SafeRunErrors SafeRunErrors;

int callMake(Program *prog);
void runProgram(Program *prog);
char **makeCmdArgs(Program *prog);
SafeRunErrors *evalExitCode(int exit);
char **diffArg(Program *prog);
char **gccBuild(Program *prog);
void printErrors(SafeRunErrors *errors, Program * prog, int testNum);
#endif
