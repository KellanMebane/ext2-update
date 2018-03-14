#include "../type.h"

int quit()
{
    int i;
    for (i = 0; i < 100; i++)
    {
        if ((minode[i]->refCount > 0) && (minode[i]->dirty == 1))
            iput(minode[i]);
    }

    exit(0);
}