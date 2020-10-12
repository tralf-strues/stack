#include "stack.h"
#include "logGenerator.h"
#include <stdlib.h>

int main()
{
    initLog();

    Stack st = {};
    stackConstruct(&st, 10); 

    stackPop(&st);

    stackPush(&st, 100); 

    //dump(&st);

    closeLog();

    return 0;
}
