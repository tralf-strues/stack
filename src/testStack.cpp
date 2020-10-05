#include "stack.h"
#include "logGenerator.h"

int main()
{
    Stack* stack = newStack();
    /*stackConstruct(&stack);*/

    initLog();

    stackPop(stack);

    for (size_t i = 0; i < 100; i++)
    {
        stackPush(stack, i);
    }

    stackDump(stack);

    closeLog();

    return 0;
}
