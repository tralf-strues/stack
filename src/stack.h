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

struct Stack
{
    size_t       size         = 0;
    size_t       capacity     = 0;
    elem_t*      dynamicArray = NULL;
    STACK_STATUS status       = NOT_CONSTRUCTED;
    STACK_ERRORS errorStatus  = NO_ERROR;
};

Stack*       stackConstruct (Stack* stack, size_t capacity);
Stack*       stackConstruct (Stack* stack);
Stack*       newStack       ();
             
bool         stackOk        (Stack* stack);    
STACK_ERRORS stackPush      (Stack* stack, elem_t value);
elem_t       stackPop       (Stack* stack);
elem_t       stackTop       (Stack* stack);
void         stackClear     (Stack* stack);    
void         stackDestruct  (Stack* stack);
void         stackDump      (Stack* stack);

