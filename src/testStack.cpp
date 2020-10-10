#include "stack.h"
#include "logGenerator.h"

int main()
{
    //Stack* stack = newStack();
    Stack yuippi = {};
    stackDefaultConstruct(yuippi);

    initLog();

    //stackPop(&yuippi);

    for (size_t i = 0; i < 13; i++)
    {
        stackPush(&yuippi, i / 10.0);
    }

    dump(&yuippi);

    //deleteStack(&yuippi);
    closeLog();

    return 0;
}
