#include "../type.h"

//need pre close

int close_from_cmd(char *pathname)
{
    int index;
    index = getFileDesc(pathname);
    if (index == -1)
    {
        printf("Error: Cannot close a file that is not open\n");
        return -1;
    }
    myclose(index);
}

int myclose(int fileDesc)
{
    MINODE *mip;
    OFT *oftp;

    if ((fileDesc >= 10) || (fileDesc < 0))
    {
        printf("Error: Invalid file descriptor\n");
        return -1;
    }
    if (running->fd[fileDesc] == 0)
    {
        printf("Error: File descriptor doesn't exist\n");
        return -1;
    }

    oftp = running->fd[fileDesc];

    if (--oftp->refCount > 0)
        return 0;
    else
    {
        mip = oftp->mptr;
        iput(mip);
    }

    running->fd[fileDesc] = 0;

    return 0;
}