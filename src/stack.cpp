#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "stack.h"
#include "logGenerator.h"

#if !defined(_DEBUG) && !defined(NDEBUG)
#define NDEBUG
#define ASSERT_STACK_OK(stack) ;
#else
#define ASSERT_STACK_OK(stack) if(stack == NULL || !stackOk(stack)) { stackDump(stack); assert(! "OK"); }
#endif

constexpr size_t STACK_EXPAND_MULTIPLIER = 2;
constexpr size_t DEFAULT_STACK_CAPACITY  = 10;

//-----------------------------------------------------------------------------
//! Stack's constructor. Allocates capacity objects of type elem_t.
//!
//! @param [out]  stack  
//! @param [in]   capacity   
//!
//! @note if calloc returned NULL then sets stack's errorStatus to 
//!       INITIALIZATION_FAILED.
//!
//! @return stack if constructed successfully or NULL otherwise.
//-----------------------------------------------------------------------------
Stack* stackConstruct(Stack* stack, size_t capacity)
{
    assert(stack != NULL);

    stack->size         = 0;
    stack->capacity     = capacity;
    stack->dynamicArray = (elem_t*) calloc(capacity, sizeof(elem_t));
    if (stack->dynamicArray == NULL)
    {
        stack->errorStatus = CONSTRUCTION_FAILED;
        #ifndef NDEBUG
        stackDump(stack);
        assert(! "OK");
        #endif
        return NULL;
    }

    ASSERT_STACK_OK(stack);

    stack->status = CONSTRUCTED;

    return stack;
}

//-----------------------------------------------------------------------------
//! Stack's constructor. Allocates DEFAULT_STACK_CAPACITY objects of type 
//! elem_t.
//!
//! @param [out]  stack   
//!
//! @note if calloc returned NULL then sets stack's errorStatus to 
//!       INITIALIZATION_FAILED.
//!
//! @return stack if constructed successfully or NULL otherwise.
//-----------------------------------------------------------------------------
Stack* stackConstruct(Stack* stack)
{
    stack = stackConstruct(stack, DEFAULT_STACK_CAPACITY);

    ASSERT_STACK_OK(stack);

    stack->status = CONSTRUCTED;

    return stack;
}

Stack* newStack()
{
    Stack* newStack = (Stack*) calloc(1, sizeof(Stack));
    stackConstruct(newStack);

    return newStack;
}

//-----------------------------------------------------------------------------
//! Checks whether or not stack is working correctly.
//!
//! @param [out]  stack   
//!
//! @return whether or not stack is working correctly.
//-----------------------------------------------------------------------------
bool stackOk(Stack* stack)
{
    assert(stack != NULL);

    if (stack->errorStatus != NO_ERROR)
    {
        return false;
    }

    if (stack->status == NOT_CONSTRUCTED)
    {
        stack->errorStatus = UNINITIALIZED_USE;

        return false;
    }

    if (stack->status == DESTRUCTED)
    {
        stack->errorStatus = POST_DESTRUCTION_USE;

        return false;
    }

    if (stack->size < 0 || stack->size > stack->capacity)
    {
        return false;
    }

    if (stack->dynamicArray == NULL)
    {
        return false;
    }

    return true;
}

STACK_ERRORS expandArray (Stack* stack, elem_t** newDynamicArray, size_t newCapacity)
{
    if (stack->status == NOT_CONSTRUCTED)
    {
        stack->errorStatus = UNINITIALIZED_USE;
        return UNINITIALIZED_USE;
    }
    ASSERT_STACK_OK(stack);

    *newDynamicArray = (elem_t*) realloc(stack->dynamicArray, newCapacity * sizeof(elem_t));
    if (*newDynamicArray == NULL)
    {
        #ifndef NDEBUG
        assert(! "OK");
        #endif

        return REALLOCATION_FAILED;
    }

    return NO_ERROR;
}

//-----------------------------------------------------------------------------
//! Push value to stack.
//!
//! @param [out]  stack   
//! @param [in]   value   
//!
//! @note if realloc returned NULL then sets stack's errorStatus to 
//!       REALLOCATION_FAILED.
//!
//! @return NO_ERROR if pushed successfully or some STACK_ERRORS code 
//!         otherwise.
//-----------------------------------------------------------------------------
STACK_ERRORS stackPush(Stack* stack, elem_t value)
{
    ASSERT_STACK_OK(stack);

    if (stack->size == stack->capacity)
    {
        elem_t* newDynamicArray = NULL;
        STACK_ERRORS error = expandArray(stack, &newDynamicArray, stack->capacity * STACK_EXPAND_MULTIPLIER);

        if (error == REALLOCATION_FAILED)
        {
            return REALLOCATION_FAILED;
        }

        stack->capacity *= STACK_EXPAND_MULTIPLIER;
        stack->dynamicArray = newDynamicArray;
    }

    stack->dynamicArray[stack->size] = value;
    stack->size++;

    ASSERT_STACK_OK(stack);

    return NO_ERROR;
}

//-----------------------------------------------------------------------------
//! Removes the element on top of the stack and returns it. Undefined behavior
//! if the stack is empty!
//!
//! @param [out]  stack    
//!
//! @note if pop is called from an empty stack then sets the stack's 
//!       errorStatus to POP_FROM_EMPTY.
//!
//! @return the element on top of the stack.
//-----------------------------------------------------------------------------
elem_t stackPop(Stack* stack)
{
    ASSERT_STACK_OK(stack);
 
    if (stack->size == 0)
    {
        stack->errorStatus = POP_FROM_EMPTY;

        #ifndef NDEBUG
        stackDump(stack);
        assert(! "OK");
        #endif
    }

    stack->size--;
    return stack->dynamicArray[stack->size];
}

//-----------------------------------------------------------------------------
//! Returns the element on top of the stack. Undefined behavior if the stack 
//! is empty!
//!
//! @param [out]  stack    
//!
//! @note if top is called from an empty stack then sets the stack's 
//!       errorStatus to TOP_FROM_EMPTY.
//!
//! @return the element on top of the stack.
//-----------------------------------------------------------------------------
elem_t stackTop(Stack* stack)
{
    ASSERT_STACK_OK(stack);

    if (stack->size == 0)
    {
        stack->errorStatus = TOP_FROM_EMPTY;

        #ifndef NDEBUG
        stackDump(stack);
        assert(! "OK");
        #endif
    }

    return stack->dynamicArray[stack->size - 1];
}

//-----------------------------------------------------------------------------
//! Empties stack.
//!
//! @param [out]  stack    
//-----------------------------------------------------------------------------
void stackClear(Stack* stack)
{
    ASSERT_STACK_OK(stack);

    stack->size = 0;
    // PUT SOME POISON!!!
}

//-----------------------------------------------------------------------------
//! Stack's destructor. Calls free for stack's dynamicArray.
//!
//! @param [out]  stack   
//-----------------------------------------------------------------------------
void stackDestruct(Stack* stack)
{
    ASSERT_STACK_OK(stack);

    stack->size         = 0;
    stack->capacity     = 0;
    free(stack->dynamicArray);
    stack->dynamicArray = NULL;

    stack->status = DESTRUCTED;
}

void stackDump(Stack* stack)
{
    LogFile logFile = {};
    /*if (!logFile.initialized)
    {
        openLogFile(&logFile);
    }*/
    openLogFile(&logFile);
    // todo

    char errorString[128];
    if (stack->errorStatus == NO_ERROR)
    {
        strcpy(errorString, "NO_ERROR");
    }
    else
    {
        strcpy(errorString, "ERROR ");

        switch(stack->errorStatus)
        {
            case POP_FROM_EMPTY:
                strcat(errorString, "1: POP_FROM_EMPTY");
            break;

            case TOP_FROM_EMPTY:
                strcat(errorString, "2: TOP_FROM_EMPTY");
            break;

            case CONSTRUCTION_FAILED:
                strcat(errorString, "3: INITIALIZATION_FAILED");
            break;

            case REALLOCATION_FAILED:
                strcat(errorString, "4: REALLOCATION_FAILED");
            break;
        }
    }
        
    fprintf(logFile.file, "Stack (%s) [0x%X] \"\"\n"
            "{\n"
            "   size         = %lld\n"
            "   capacity     = %lld\n"
            "   dynamicArray [0x%X]\n"
            "   {\n",
            errorString, stack, stack->size, stack->capacity, stack->dynamicArray);

    for (size_t i = 0; i < stack->capacity; i++)
    {
        if (i < stack->size)
        {
            // todo printing elem_t
            fprintf(logFile.file, 
                    "       *[%lld] = %g\n", 
                    i, stack->dynamicArray[i]);
        }
        else
        {
            // todo printing elem_t
            fprintf(logFile.file, 
                    "        [%lld] = %g\n", 
                    i, stack->dynamicArray[i]);
        }
    }

    fprintf(logFile.file, 
            "   }\n"
            "}\n");

    closeLogFile(&logFile);
}