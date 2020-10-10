#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "stack.h"
#include "logGenerator.h"

//#define PRINT_ERROR(error) strcat(errorString, ((int)error + '0')); strcat(errorString, #error);

#ifdef STACK_POISON
#define PUT_POISON(begin, end) putPoison(begin, end)

//-----------------------------------------------------------------------------
//! Sets values [begin, end) to POISON.
//!
//! @param [in]  begin  
//! @param [in]  end   
//-----------------------------------------------------------------------------
void putPoison(elem_t* begin, elem_t* end)
{
    for (; begin < end; begin++)
    {
        *begin = STACK_POISON;
    }
}

#else
#define PUT_POISON(begin, end) 
#endif

#ifdef STACK_CANARIES_ENABLED
#define GET_CANARY(memBlock, memBlockSize, side)               getCanary  (memBlock, memBlockSize, side)
#define SET_CANARY(memBlock, memBlockSize, canary, side)       setCanary  (memBlock, memBlockSize, canary,  side)
#define SET_CANARIES(memBlock, memBlockSize, canaryL, canaryR) setCanaries(memBlock, memBlockSize, canaryL, canaryR)

uint32_t getCanary(void* memBlock, size_t memBlockSize, char side)
{
    if (side == 'l')
    {
        return *(uint32_t*)((char*) memBlock - sizeof(uint32_t));
    }
    else if (side == 'r')
    {
        return *(uint32_t*)((char*) memBlock + memBlockSize);
    }

    return 0;
}

void setCanary(void* memBlock, size_t memBlockSize, uint32_t canary, char side)
{
    if (side == 'l')
    {
        *(uint32_t*)((char*) memBlock - sizeof(uint32_t)) = canary;
    }
    else if (side == 'r')
    {
        *(uint32_t*)((char*) memBlock + memBlockSize)  = canary;
    }
}

void setCanaries(void* memBlock, size_t memBlockSize, uint32_t canaryL, uint32_t canaryR)
{
    setCanary(memBlock, memBlockSize, canaryL, 'l');
    setCanary(memBlock, memBlockSize, canaryR, 'r');
}

#else
#define GET_CANARY(memBlock, memBlockSize, side)               
#define SET_CANARY(memBlock, memBlockSize, canary, side)       
#define SET_CANARIES(memBlock, memBlockSize, canaryL, canaryR) 
#endif

//-----------------------------------------------------------------------------
//! Stack's constructor. Allocates max(capacity, MINIMAL_STACK_CAPACITY) 
//! objects of type elem_t.
//!
//! @param [out]  stack  
//! @param [in]   capacity   
//!
//! @note if calloc returned NULL then sets stack's errorStatus to 
//!       INITIALIZATION_FAILED.
//!
//! @return stack if constructed successfully or NULL otherwise.
//-----------------------------------------------------------------------------
#ifdef STACK_DEBUG_MODE
Stack* fstackConstruct(Stack* stack, size_t capacity, const char* stackName)
#else
Stack* fstackConstruct(Stack* stack, size_t capacity)
#endif
{
    assert(stack != NULL);
    assert(capacity > 0);

    #ifdef STACK_DEBUG_MODE
    stack->name         = stackName;
    #endif

    stack->size         = 0;
    stack->capacity     = capacity > MINIMAL_STACK_CAPACITY ? capacity : MINIMAL_STACK_CAPACITY;

    #ifdef STACK_CANARIES_ENABLED
    stack->dynamicArray = (elem_t*) ((char*)calloc(1, stack->capacity * sizeof(elem_t) + 
                                                      sizeof(STACK_ARRAY_CANARY_L) + 
                                                      sizeof(STACK_ARRAY_CANARY_R)) 
                                     + sizeof(STACK_ARRAY_CANARY_L));
    #else
    stack->dynamicArray = (elem_t*) calloc(stack->capacity, sizeof(elem_t));
    #endif

    if (stack->dynamicArray == NULL) 
    {
        stack->errorStatus = CONSTRUCTION_FAILED;
        ASSERT_STACK_OK(stack);
        return NULL;
    }

    SET_CANARIES((void*)stack->dynamicArray, stack->capacity * sizeof(elem_t), STACK_ARRAY_CANARY_L, STACK_ARRAY_CANARY_R);
    SET_CANARIES((void*)(stack + 1), sizeof(Stack), STACK_STRUCT_CANARY_L, STACK_ARRAY_CANARY_R);

    PUT_POISON(stack->dynamicArray, stack->dynamicArray + stack->capacity);
    stack->status = CONSTRUCTED;
    ASSERT_STACK_OK(stack);

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
#ifdef STACK_DEBUG_MODE
Stack* fstackConstruct(Stack* stack, const char* stackName)
#else
Stack* fstackConstruct(Stack* stack)
#endif
{
    #ifdef STACK_DEBUG_MODE
    stack = fstackConstruct(stack, DEFAULT_STACK_CAPACITY, stackName);
    #else
    stack = fstackConstruct(stack, DEFAULT_STACK_CAPACITY);
    #endif

    ASSERT_STACK_OK(stack);

    return stack;
}

//-----------------------------------------------------------------------------
//! Allocates a Stack, calls constructor and returns the pointer to this Stack.
//!
//! @param [in]  capacity   
//!
//! @note if calloc returned NULL then sets stack's errorStatus to 
//!       INITIALIZATION_FAILED.
//! @note if capacity is less than MINIMAL_STACK_CAPACITY, than sets capacity
//!       to MINIMAL_STACK_CAPACITY.
//!
//! @return stack if constructed successfully or NULL otherwise.
//-----------------------------------------------------------------------------
Stack* newStack(size_t capacity)
{
    assert(capacity > 0);

    Stack* newStack = (Stack*) calloc(1, sizeof(Stack));

    #ifdef STACK_DEBUG_MODE
    fstackConstruct(newStack, DYNAMICALLY_CREATED_STACK_NAME);
    #else
    fstackConstruct(newStack);
    #endif

    return newStack;
}

//-----------------------------------------------------------------------------
//! Allocates a Stack, calls constructor and returns the pointer to this Stack.
//!
//! @note if calloc returned NULL then sets stack's errorStatus to 
//!       INITIALIZATION_FAILED.
//! @note starting capacity is DEFAULT_STACK_CAPACITY
//!
//! @return stack if constructed successfully or NULL otherwise.
//-----------------------------------------------------------------------------
Stack* newStack()
{
    return newStack(DEFAULT_STACK_CAPACITY);
}

//-----------------------------------------------------------------------------
//! Stack's destructor. Calls free for stack's dynamicArray.
//!
//! @param [out]  stack   
//-----------------------------------------------------------------------------
void stackDestruct(Stack* stack)
{
    ASSERT_STACK_OK(stack);
    PUT_POISON(stack->dynamicArray, stack->dynamicArray + stack->capacity);

    #ifdef STACK_CANARIES_ENABLED
    free((char*)stack->dynamicArray - sizeof(STACK_ARRAY_CANARY_L));
    #else
    free(stack->dynamicArray);
    #endif

    stack->size         = 0;
    stack->capacity     = 0;
    stack->dynamicArray = NULL;

    stack->status = DESTRUCTED;
}

//-----------------------------------------------------------------------------
//! Calls destructor of stack and free of stack. Undefined behavior if stack
//! wasn't created dynamically using newStack() or an allocator.
//!
//! @param [out]  stack   
//-----------------------------------------------------------------------------
void deleteStack(Stack* stack)
{
    ASSERT_STACK_OK(stack);

    stackDestruct(stack);

    free(stack);
}

//-----------------------------------------------------------------------------
//! @param [in]  stack   
//!
//! @return current size of stack.
//-----------------------------------------------------------------------------
size_t stackSize(Stack* stack)
{
    ASSERT_STACK_OK(stack);

    return stack->size;
}

//-----------------------------------------------------------------------------
//! @param [in]  stack   
//!
//! @return current capacity of stack.
//-----------------------------------------------------------------------------
size_t stackCapacity(Stack* stack)
{
    ASSERT_STACK_OK(stack);

    return stack->capacity;
}

//-----------------------------------------------------------------------------
//! @param [in]  stack   
//!
//! @return current stack's errorStatus.
//-----------------------------------------------------------------------------
STACK_ERRORS stackErrorStatus (Stack* stack)
{
    assert(stack != NULL);

    return stack->errorStatus;
}

//-----------------------------------------------------------------------------
//! Resizes stack's array to newCapacity. If reallocation was unsuccessful, 
//! returns NULL, sets stack's errorStatus to REALLOCATION_FAILED, but current
//! elements of the stack won't be removed.
//!
//! @param [out]  stack   
//! @param [in]   newCapacity   
//!
//! @note if newCapacity is less than stack's size then there will be DATA 
//!       LOSS!
//!
//! @return pointer to the new stack's array if reallocation was successful 
//!         or NULL otherwise.
//-----------------------------------------------------------------------------
elem_t* resizeArray(Stack* stack, size_t newCapacity)
{
    elem_t* newDynamicArray = NULL;
    ASSERT_STACK_OK(stack);

    #ifdef STACK_CANARIES_ENABLED
    newDynamicArray = (elem_t*) ((char*) realloc((char*) stack->dynamicArray - sizeof(STACK_ARRAY_CANARY_L), 
                                                 newCapacity * sizeof(elem_t) + 
                                                 sizeof(STACK_ARRAY_CANARY_L) + 
                                                 sizeof(STACK_ARRAY_CANARY_R)) + sizeof(STACK_ARRAY_CANARY_L));
    #else
    newDynamicArray = (elem_t*) realloc(stack->dynamicArray, newCapacity * sizeof(elem_t));
    #endif

    if (newDynamicArray == NULL)
    {
        stack->errorStatus = REALLOCATION_FAILED;
        ASSERT_STACK_OK(stack);
    }
    else
    {
        stack->dynamicArray = newDynamicArray;
        stack->capacity     = newCapacity;

        PUT_POISON(stack->dynamicArray + stack->size, stack->dynamicArray + stack->capacity);

        SET_CANARY((void*)stack->dynamicArray, stack->capacity * sizeof(elem_t), STACK_ARRAY_CANARY_R, 'r');
    }

    return newDynamicArray;
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
        elem_t* newDynamicArray = resizeArray(stack, stack->capacity * STACK_EXPAND_MULTIPLIER);

        if (newDynamicArray == NULL)
        {
            return REALLOCATION_FAILED;
        }
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
        ASSERT_STACK_OK(stack);
    }

    stack->size--;
    elem_t returnValue = stack->dynamicArray[stack->size];

    PUT_POISON(stack->dynamicArray + stack->size, stack->dynamicArray + stack->size + 1);

    return returnValue;
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
        ASSERT_STACK_OK(stack);
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

    if (!stackShrinkToFit(stack))
    {
        stack->errorStatus = REALLOCATION_FAILED;
        ASSERT_STACK_OK(stack);
    }

    PUT_POISON(stack->dynamicArray, stack->dynamicArray + stack->capacity);

    ASSERT_STACK_OK(stack);
}

//-----------------------------------------------------------------------------
//! Resizes stack's array to max(stack's size, MINIMAL_STACK_CAPACITY). If 
//! reallocation was unsuccessful, returns false, sets stack's errorStatus to 
//! REALLOCATION_FAILED, but current elements of the stack won't be removed.
//!
//! @param [out]  stack   
//!
//! @return whether or not shrinking was successful.
//-----------------------------------------------------------------------------
bool stackShrinkToFit(Stack* stack)
{
    ASSERT_STACK_OK(stack);

    if (stack->size <= MINIMAL_STACK_CAPACITY)
    {
        return false;
    }

    elem_t* newDynamicArray = resizeArray(stack, stack->size);
    if (newDynamicArray == NULL)
    {
        stack->errorStatus = REALLOCATION_FAILED;
        ASSERT_STACK_OK(stack);
        return false;
    }

    return true;
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

    if (stack == NULL)
    {
        return false;
    }

    if (stack->errorStatus != NO_ERROR)
    {
        return false;
    }

    if (stack->status == NOT_CONSTRUCTED)
    {
        stack->errorStatus = NOT_CONSTRUCTED_USE;
        return false;
    }

    if (stack->status == DESTRUCTED)
    {
        stack->errorStatus = DESTRUCTED_USE;
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

    #ifdef STACK_POISON
    for (size_t i = 0; i < stack->size; i++)
    {
        if (IS_STACK_POISON(stack->dynamicArray[i]))
        {
            stack->errorStatus = MEMORY_CORRUPTION;
            return false;
        }
    }

    for (size_t i = stack->size; i < stack->capacity; i++)
    {
        if (!IS_STACK_POISON(stack->dynamicArray[i]))
        {
            stack->errorStatus = MEMORY_CORRUPTION;
            return false;
        }
    }
    #endif

    #ifdef STACK_CANARIES_ENABLED
    if (getCanary((void*)stack->dynamicArray, stack->capacity * sizeof(elem_t), 'l') != STACK_ARRAY_CANARY_L  || 
        getCanary((void*)stack->dynamicArray, stack->capacity * sizeof(elem_t), 'r') != STACK_ARRAY_CANARY_R  ||
        stack->canaryL                                                               != STACK_STRUCT_CANARY_L ||
        stack->canaryR                                                               != STACK_STRUCT_CANARY_R)
    {
        stack->errorStatus = MEMORY_CORRUPTION;
        return false;
    }
    #endif

    return true;
}

//-----------------------------------------------------------------------------
//! Uses logGenerator to dump stack to html log file. 
//!
//! @param [out]  stack   
//-----------------------------------------------------------------------------
void dump(Stack* stack)
{
    if (!isLogInitialized())
    {
        initLog();
    }

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
                strcat(errorString, "3: CONSTRUCTION_FAILED");
            break;

            case REALLOCATION_FAILED:
                strcat(errorString, "4: REALLOCATION_FAILED");
            break;

            case NOT_CONSTRUCTED_USE:
                strcat(errorString, "5: NOT_CONSTRUCTED_USE");
            break;

            case DESTRUCTED_USE:
                strcat(errorString, "6: DESTRUCTED_USE");
            break;

            case MEMORY_CORRUPTION:
                strcat(errorString, "7: MEMORY_CORRUPTION");
            break;
        }
    }
    
    logWriteMessageStart(LOG_COLOR_BLACK);
    logWrite("Stack (");
    logWrite(errorString, stack->errorStatus == NO_ERROR ? LOG_COLOR_GREEN : LOG_COLOR_RED);
    logWrite(") [0x%X] "

             #ifdef STACK_DEBUG_MODE
             "\"%s\""
             #endif

             "\n"
             "{\n"  

             #ifdef STACK_CANARIES_ENABLED
             "   canaryL: 0x%lX | must be 0x%lX\n"
             "   canaryR: 0x%lX | must be 0x%lX\n\n"
             #endif

             "   size         = %llu\n"
             "   capacity     = %llu\n"
             "   dynamicArray [0x%X]\n"
             "   {\n"

             #ifdef STACK_CANARIES_ENABLED
             "       canaryL: 0x%lX | must be 0x%lX\n"
             "       canaryR: 0x%lX | must be 0x%lX\n\n"
             #endif

             ,
             stack, 

             #ifdef STACK_DEBUG_MODE
             stack->name, 
             #endif

             #ifdef STACK_CANARIES_ENABLED
             stack->canaryL, STACK_STRUCT_CANARY_L,
             stack->canaryR, STACK_STRUCT_CANARY_R,
             #endif

             stack->size, 
             stack->capacity, 
             stack->dynamicArray
             
             #ifdef STACK_CANARIES_ENABLED
             ,getCanary((void*)stack->dynamicArray, stack->capacity * sizeof(elem_t), 'l'),
              STACK_ARRAY_CANARY_L,
              getCanary((void*)stack->dynamicArray, stack->capacity * sizeof(elem_t), 'r'),
              STACK_ARRAY_CANARY_R
             #endif  
    );

    for (size_t i = 0; i < stack->capacity; i++)
    {
        if (i < stack->size)
        {
            // todo printing elem_t
            logWrite("       *[%llu]\t= %f ", 
                     i, stack->dynamicArray[i]);
        }
        else
        {
            // todo printing elem_t
            logWrite("        [%llu]\t= %f ", 
                     i, stack->dynamicArray[i]);
        }

        #ifdef STACK_POISON
        if (IS_STACK_POISON(stack->dynamicArray[i]))
        {
            logWrite("(POISON!)");
        }
        #endif

        logWrite("\n");
    }

    logWrite("   }\n"
             "}\n");

    logWriteMessageEnd();

    if (stack->errorStatus != NO_ERROR)
    {
        closeLog();
    }
}
