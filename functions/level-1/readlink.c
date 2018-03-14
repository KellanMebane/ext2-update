#include "../type.h"

int myreadlink(char *filename, char **buffer)
{
    MINODE *mip;
    int ino, res, fd = root->dev;
    char buf[1024], *cp, temp[64];
    DIR *dp;

    ino = getino(&fd, filename);
    if (ino < 2)
    {
        printf("Error: path \"%s\" doesn't exist!\n", filename);
        return;
    }
    mip = iget(fd, ino);

    if (permissionCheck(mip))
        return -1;

    if (mip->INODE.i_mode != 0xA1a4)
    {
        iput(mip);
        printf("Error: path \"%s\" isn't a link!\n", filename);
        return;
    }

    //printf("\nblock number = %d\n", mip->INODE.i_block[0]);
    get_block(mip->dev, mip->INODE.i_block[0], buf);
    dp = (DIR *)buf;

    //printf("hello\n");

    strcpy(temp, dp->name);
    temp[dp->name_len] = 0;
    *buffer = temp;
    res = dp->name_len;

    //printf("name = %s size = %d\n", buffer, res);

    iput(mip);
    put_block(fd, mip->INODE.i_block[0], buf);

    return res;
}