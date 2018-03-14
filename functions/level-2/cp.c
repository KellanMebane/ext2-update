#include "../type.h"

int mycp(char *source, char *dest)
{
    int fsrc, fdest, n = 1024;
    char buf[BLKSIZE];

    fsrc = myopen(source, 0); //open for read
    fdest = myopen(dest, 1);  //open for write

    if (fsrc == -1 || fdest == -1)
    {
        myclose(fsrc);
        myclose(fdest);
        return -1;
    }

    if (running->fd[fsrc]->mptr == running->fd[fdest]->mptr)
    {
        printf("Error: Cannot cp to same file!\n");
        myclose(fsrc);
        myclose(fdest);
        return -1;
    }

    while (n > 0)
    {
        n = myk_read(fsrc, buf, n, 0);
        mywrite_file(fdest, buf, n, 0);
    }

    myclose(fsrc);
    myclose(fdest);
    return 0;
}
