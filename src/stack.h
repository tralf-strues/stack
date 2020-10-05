#pragma once

#include <stdio.h>

typedef double elem_t;

enum STACK_ERRORS
{
    NO_ERROR,
    POP_FROM_EMPTY,
    TOP_FROM_EMPTY,
    CONSTRUCTION_FAILED,
    REALLOCATION_FAILED,
    UNINITIALIZED_USE,
    POST_DESTRUCTION_USE
};

enum STACK_STATUS
{
    NOT_CONSTRUCTED,
    CONSTRUCTED,
    DESTRUCTED
};

struct Stack;

Stack*       stackConstruct   (Stack* stack, size_t capacity);
Stack*       stackConstruct   (Stack* stack);
Stack*       newStack         (size_t capacity);
Stack*       newStack         ();
void         stackDestruct    (Stack* stack);
void         deleteStack      (Stack** stack);
                              
STACK_ERRORS stackPush        (Stack* stack, elem_t value);
elem_t       stackPop         (Stack* stack);
elem_t       stackTop         (Stack* stack);
void         stackClear       (Stack* stack);
void         stackShrinkToFit (Stack* stack);

bool         stackOk          (Stack* stack);    
void         stackDump        (Stack* stack);

