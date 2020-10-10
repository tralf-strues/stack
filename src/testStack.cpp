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

    yuippi.dynamicArray[3] = 0.4;

    ASSERT_STACK_OK(&yuippi);

    //deleteStack(&yuippi);
    closeLog();

    return 0;
}
