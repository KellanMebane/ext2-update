#include "../type.h"

int mytouch(char *pathname)
{
    MINODE *mip;
    int ino, fd = root->dev;
    time_t curtime;

    ino = getino(&fd, pathname);
    if (ino < 2)
    {
        printf("Error: path \"%s\" doesn't exist!\n", pathname);
        return -1;
    }
    mip = iget(fd, ino);

    mip->INODE.i_atime = time(0L);
    mip->dirty = 1;
    iput(mip);

    return 0;
}