#ifndef STACK_H
#define STACK_H

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#ifdef STACK_DEBUG_MODE
#define STACK_DEBUG_LVL3 
#endif

#if !defined(STACK_DEBUG_LVL1) && !defined(STACK_DEBUG_LVL2) && !defined(STACK_DEBUG_LVL3)
#define ASSERT_STACK_OK(stack) assert(stack != NULL);
#else
#define STACK_DEBUG_MODE
#endif

#ifdef STACK_DEBUG_LVL1
#define ASSERT_STACK_OK(stack) if(stack == NULL || !stackOk(stack)) { dump(stack); assert(! "OK"); }
#define STACK_POISON           nan("")
#define IS_STACK_POISON(value) isnan(value)
#endif

#ifdef STACK_DEBUG_LVL2
#define ASSERT_STACK_OK(stack) if(stack == NULL || !stackOk(stack)) { dump(stack); assert(! "OK"); }
#define STACK_POISON           nan("")
#define IS_STACK_POISON(value) isnan(value)

#define STACK_CANARIES_ENABLED
#endif

#ifdef STACK_DEBUG_LVL3
#define ASSERT_STACK_OK(stack) if(stack == NULL || !stackOk(stack)) { dump(stack); assert(! "OK"); }
#define STACK_POISON           nan("")
#define IS_STACK_POISON(value) isnan(value)
#define STACK_CANARIES_ENABLED

#define STACK_ARRAY_HASHING
#endif

#ifdef STACK_CANARIES_ENABLED
static uint32_t STACK_ARRAY_CANARY_L  = 0xBADC0FFE;
static uint32_t STACK_ARRAY_CANARY_R  = 0xDEADBEEF;

static uint32_t STACK_STRUCT_CANARY_L = 0xDEDDED32;
static uint32_t STACK_STRUCT_CANARY_R = 0xFACEBEEF;
#endif

#ifdef STACK_DEBUG_MODE
#define stackConstruct(stack, capacity) fstackConstruct(stack, capacity, &#stack[1]);
#define stackDefaultConstruct(stack)    fstackConstruct(stack, &#stack[1]);
#else
#define stackConstruct(stack, capacity) fstackConstruct(stack, capacity);
#define stackDefaultConstruct(stack)    fstackConstruct(stack);
#endif

typedef double elem_t;

static double STACK_EXPAND_MULTIPLIER = 1.8;
static size_t DEFAULT_STACK_CAPACITY  = 10;
static size_t MINIMAL_STACK_CAPACITY  = 3;

#ifdef STACK_DEBUG_MODE
static const char* DYNAMICALLY_CREATED_STACK_NAME  = "no name, created dynamically";
#endif

enum StackErrors
{
    STACK_NO_ERROR,
    STACK_POP_FROM_EMPTY,
    STACK_TOP_FROM_EMPTY,
    STACK_CONSTRUCTION_FAILED,
    STACK_REALLOCATION_FAILED,
    STACK_NOT_CONSTRUCTED_USE,
    STACK_DESTRUCTED_USE,
    STACK_MEMORY_CORRUPTION
};

enum StackStatus
{
    STACK_STATUS_NOT_CONSTRUCTED,
    STACK_STATUS_CONSTRUCTED,
    STACK_STATUS_DESTRUCTED
};

struct Stack
{
    #ifdef STACK_CANARIES_ENABLED
    uint32_t canaryL = STACK_STRUCT_CANARY_L;
    #endif

    #ifdef STACK_DEBUG_MODE
    const char* name = NULL;
    #endif

    size_t       size         = 0;
    size_t       capacity     = 0;
    elem_t*      dynamicArray = NULL;
    StackStatus  status       = STACK_STATUS_NOT_CONSTRUCTED;
    StackErrors  errorStatus  = STACK_NO_ERROR;

    #ifdef STACK_CANARIES_ENABLED
    uint32_t canaryR = STACK_STRUCT_CANARY_R;
    #endif
};

#ifdef STACK_DEBUG_MODE
Stack*       fstackConstruct  (Stack* stack, size_t capacity, const char* stackName);
Stack*       fstackConstruct  (Stack* stack, const char* stackName);
#else
Stack*       fstackConstruct  (Stack* stack, size_t capacity);
Stack*       fstackConstruct  (Stack* stack);
#endif

Stack*       newStack         (size_t capacity);
Stack*       newStack         ();
void         stackDestruct    (Stack* stack);
void         deleteStack      (Stack* stack);
size_t       stackSize        (Stack* stack);
size_t       stackCapacity    (Stack* stack);
StackErrors  stackErrorStatus (Stack* stack);
                              
StackErrors  stackPush        (Stack* stack, elem_t value);
elem_t       stackPop         (Stack* stack);
elem_t       stackTop         (Stack* stack);
void         stackClear       (Stack* stack);
bool         stackShrinkToFit (Stack* stack);

bool         stackOk          (Stack* stack);    
void         dump             (Stack* stack);

#endif