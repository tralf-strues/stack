#include "stack.h"
#include "logGenerator.h"
#include <stdlib.h>

int main()
{
    //Stack* stack = newStack();
    Stack yuippi;
    stackConstruct(yuippi, 10);
    //stackDefaultConstruct(yuippi);

    //initLog();

    ////stackPop(&yuippi);

    //for (size_t i = 0; i < 13; i++)
    //{
    //    stackPush(&yuippi, i / 10.0);
    //}

    //yuippi.dynamicArray[3] = 0.3;

    //ASSERT_STACK_OK(&yuippi);

    dump(&yuippi);

    ////deleteStack(&yuippi);
    //closeLog();

    return 0;
}
