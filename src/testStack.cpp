#include "stack.h"

int main()
{
    Stack stack = {};
    stackConstruct(&stack);

    //stackPop(&stack);

    for (size_t i = 0; i < 100; i++)
    {
        stackPush(&stack, i);
    }

    stackDump(&stack);

    return 0;
}
