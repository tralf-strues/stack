#include "stack.h"

int main()
{
    Stack stack = {};
    stackConstruct(&stack, 16);

    stackPop(&stack);

    stackDestruct(&stack);
}