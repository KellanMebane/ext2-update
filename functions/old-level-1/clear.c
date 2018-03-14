#include "../type.h"

void myclear()
{
    for (int i = 0; i < 200; i++)
        printf("\n");

    printf("\033[2J");
}