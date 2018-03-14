#include "../type.h"

void mychmod(char *filename, char *chmode)
{
    MINODE *mip;
    int ino, octmode, fd = root->dev;

    ino = getino(&fd, filename);
    if (ino < 2)
    {
        printf("Error: path \"%s\" doesn't exist!\n", filename);
        return;
    }
    mip = iget(fd, ino);

    sscanf(chmode, "%o", &octmode);

    if ((mip->INODE.i_mode & 0100000) == 0100000)
        mip->INODE.i_mode = 0100000 + octmode;
    else if ((mip->INODE.i_mode & 0040000) == 0040000)
        mip->INODE.i_mode = 0040000 + octmode;
    else if ((mip->INODE.i_mode & 0120000) == 0120000)
        mip->INODE.i_mode = 0120000 + octmode;

    mip->dirty = 1;
    iput(mip);
}