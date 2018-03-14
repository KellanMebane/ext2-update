#include "../type.h"


int mycd(char *pathname)
{
    int ino, fd = root->dev;
    MINODE *mip;

    if (pathname[0] == 0)
    {
        running->cwd = root;
        return 0;
    }

    ino = getino(&fd, pathname);
    mip = iget(fd, ino);

    if ((ino < 2) || (mip == 0)) //counld't find dir or went off inode track
    {
        printf("Error: failed to find directory %s!\n", pathname);
        iput(mip);
        return -1;
    }

    if (mip->INODE.i_mode != 0x41ed) //is dir? //don't do anything with lost+found for now
    {                                //no
        printf("Error: %s isn't a directory!\n", pathname);
        iput(mip);
        return -1;
    }

    running->cwd = iget(fd, mip->ino);
    //iput(running->cwd);
    iput(mip);

    return 0;
}