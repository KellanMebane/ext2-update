#include "../type.h"

//need pre write

int write_from_cmd(char *path, char *ppath, char *pppath) //file text nbytes
{
    int n, nbytes, index;
    sscanf(pppath, "%d", &nbytes);
    index = getFileDesc(path);
    if (index == -1)
    {
        printf("Error: Cannot read a file that is not open!\n");
        return -1;
    }

    n = myk_write(index, ppath, nbytes, 0);

    return n;
}

int myk_write(int fileDesc, char buf[], long nbytes, int space)
{
    //step 1
    if (fileDesc < 0 || fileDesc > 9) //FD's from 0->9
    {
        printf("Error: Invalid file descriptor!\n");
        return -1;
    }
    if ((running->fd[fileDesc]->mode != 1) &&
        (running->fd[fileDesc]->mode != 2) && (running->fd[fileDesc]->mode != 3)) //check mode for W RW APP
    {
        printf("Error: File is not in a write ready mode!\n");
        return -1;
    }

    //skip 2 & 3
    //step 4
    if ((running->fd[fileDesc]->mptr->INODE.i_mode & 0xF000) != 0x8000)
    {
        printf("Error: Program can only write to regular files!\n");
        return -1;
    }

    return mywrite_file(fileDesc, buf, nbytes, space);
}

int mywrite_file(int fileDesc, char *buf, long nbytes, int space)
{
    long count = 0, lbk, start, blk, remain = 0, length;
    char kbuf[BLKSIZE];
    u32 ibuf[256], dbuf[256], empty[256];
    char *cp, *cq;
    cq = (char *)buf;
    OFT *oftp = running->fd[fileDesc];
    MINODE *mip = running->fd[fileDesc]->mptr;
    int indirect = 0, doubleIndirect = 0;
    long doubledouble = 0;


    length = strlen(buf);
    if (nbytes > strlen)
        nbytes = strlen;

    if (nbytes > 8192) //cap amount to write
        nbytes = 8192;


    while (nbytes > 0)
    {
        lbk = oftp->offset / BLKSIZE;
        start = oftp->offset % BLKSIZE;

        //map and allocate
        if (lbk < 12) //direct
        {
            if (mip->INODE.i_block[lbk] == 0)
                mip->INODE.i_block[lbk] = balloc(mip->dev);
            blk = mip->INODE.i_block[lbk];
        }
        else if (12 <= lbk && lbk < 12 + 256) //indirect
        {
            if (!indirect)
            {
                get_block(mip->dev, mip->INODE.i_block[12], ibuf);
                indirect = 1;
            }
            blk = empty[lbk - 12];
        }
        else //double indirect
        {
            if (!doubleIndirect)
            {
                get_block(mip->dev, mip->INODE.i_block[13], dbuf);
                doubleIndirect = 1;
            }
            if (doubledouble != dbuf[((lbk - 268) / 256)])
            {
                doubledouble = /**(dindi + ((lbk - 268) / 256));*/dbuf[((lbk - 268) / 256)];
                get_block(mip->dev, doubledouble, ibuf);
            }
            blk = empty[((lbk - 268) % 256)];
        }


        if (blk < 1 || blk > 1440)
        {
            //printf("block number = %d\n", blk);
            blk = 0;
        }
        //printf("block number = %d\n", blk);

        get_block(mip->dev, blk, kbuf);
        if (kbuf[0] == 0)
            memset(kbuf, BLKSIZE, 0);
        cp = kbuf + start;
        remain = BLKSIZE - start;
        while (remain > 0)
        {
            *cp++ = *cq++;

            oftp->offset++;
            count++;
            remain--;
            nbytes--;

            if (oftp->offset > mip->INODE.i_size)
                mip->INODE.i_size++;

            if (nbytes <= 0)
                break;
        }
        put_block(mip->dev, blk, kbuf);
    }
    
    mip->dirty = 1;
    iput(mip);
    return count;
}
