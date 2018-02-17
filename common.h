#ifndef COMMON_H
#define COMMON_H
#include <execinfo.h>
#include <stdlib.h>
#include <stdio.h>

#define print_stacktrace()\
do{\
    int size = 128;\
    void * array[128];\
    int stack_num = backtrace(array, size);\
    char ** stacktrace = backtrace_symbols(array, stack_num);\
    for (int i = 0; i < stack_num; ++i)\
    {\
        printf("%s\n", stacktrace[i]);\
    }\
    free(stacktrace);\
}while(0)

#endif // COMMON_H
