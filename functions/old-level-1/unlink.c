#include "../type.h"

int myunlink(char *pathname)
{
    int ino, pino, i = 0, fd = root->dev;
    char *temp1, *temp2, *parent, *child;
    MINODE *mip, *pmip;

    ino = getino(&fd, pathname); //get inode of existing file
    if (ino < 2)            //if it's less than root return
    {
        printf("path %s not on disk!\n", pathname);
        return -1;
    }
    mip = iget(fd, ino);
    if (mip->INODE.i_mode == 0x41ED) //check specific directory, later on when user mode
    {                                // is added we'll need to change this to
                                     //check first bit for 0x4000
        printf("Error: \"%s\" is a directory, cannot unlink!\n", child);
        iput(mip);
        return -1;
    }

    temp1 = strdup(pathname); //break down into parent dir and new dir
    parent = dirname(temp1);
    temp2 = strdup(pathname);
    child = basename(temp2);

    fd = root->dev;
    pino = getino(&fd, parent);
    if (ino < 2) //if it's less than root return
    {
        printf("path %s not on disk!\n", pathname);
        iput(mip);
        return -1;
    }
    pmip = iget(fd, pino);
    if (pmip->INODE.i_mode != 0x41ED) //check specific directory, later on when user mode
    {                                 // is added we'll need to change this to
                                      //check first bit for 0x4000
        printf("Error: \"%s\" is a directory, cannot unlink!\n", parent);
        iput(mip);
        iput(pmip);
        return -1;
    }

    rm_child(pmip, child);
    pmip->dirty = 1;
    iput(pmip);

    mip->INODE.i_links_count--;

    if (mip->INODE.i_links_count > 0)
        mip->dirty = 1;
    else
    {
        //dalloc blocks
        for (i = 0; i < 15; i++)
        {
            bdalloc(fd, mip->INODE.i_block[i]);
        }
        idalloc(fd, ino);
    }
    iput(mip);

    return 0;
}