#include "../type.h"


int myrmdir(char *pathname)
{
    char *temp1, *temp2, *parent, *child, buf[BLKSIZE], *cp;
    int pino, ino, blk, temp_rec_len, n_entries = 0, fd = root->dev;
    MINODE *pmip, *mip;
    DIR *dp, *tdp;
    int i = 0;

    temp1 = strdup(pathname); //break down into parent dir and new dir
    parent = dirname(temp1);
    temp2 = strdup(pathname);
    child = basename(temp2);

    printf("pathname = %s basename = %s\n", parent, child);

    ino = getino(&fd, child);   //find ino of child
    mip = iget(fd, ino);   //get child node
    fd = root->dev;
    pino = getino(&fd, parent); //find ino of parent
    pmip = iget(fd, pino); //get parent node

    if (mip->INODE.i_mode != 0x41ED) //check specific directory, later on when user mode
    {                                // is added we'll need to change this to
                                     //check first bit for 0x4000
        printf("Error: \"%s\" is not a directory!\n", child);
        return -1;
    }

    if (mip->refCount > 1)
    {
        printf("Error: \"%s\" is busy!\n", child);
        return -1;
    }

    if (search(pmip, child) == 0) //if the child exist we can't make it
    {
        printf("Error: file \"%s\" doesn't exist!\n", child);
        return -1;
    }

    for (i = 0; i < 15; i++)
    {
        if (mip->INODE.i_block[i] == 0)
            break;

        get_block(mip->dev, mip->INODE.i_block[i], buf);
        tdp = (DIR *)buf;
        cp = buf;

        while (cp < buf + BLKSIZE) //second pointers iterate to last record
        {
            n_entries++;

            cp += tdp->rec_len;
            tdp = (DIR *)cp;
        }

        if (n_entries > 2)
        {
            printf("Error: directory \"%s\" isn't empty!\n", child);
            return -1;
        }
    }

    bdalloc(mip->dev, mip->INODE.i_block[0]);
    idalloc(mip->dev, mip->ino);
    iput(mip);
    rm_child(pmip, child);
    pmip->INODE.i_links_count--;
    pmip->dirty = 1;
    iput(pmip);

    return 0;
}