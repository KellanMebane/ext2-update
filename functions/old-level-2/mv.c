#include "../type.h"

int mymv(char *source, char *dest)
{
    if (mycp(source, dest) == -1)
    {
        printf("Error: Cannot move file!\n");
        return -1;
    }

    int nfd = myopen(source, 1);
    myclose(nfd);
    myunlink(source);

    return 0;
}