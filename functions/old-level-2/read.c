#include "../type.h"


//need a pre read

int read_from_cmd(char *path, char *ppath)
{
    int n, nbytes, index;
    char buf[8196]; //max read 8196 bytes
    sscanf(ppath, "%d", &nbytes);
    index = getFileDesc(path);
    if (index == -1)
    {
        printf("Error: Cannot read a file that is not open!\n");
        return -1;
    }

    n = myk_read(index, buf, nbytes, 0);

    buf[n] = '\0';

    printf("%s\n", buf);

    return n;
}



int myk_read(int fileDesc, char buf[], long nbytes, int space)
{
    //step 1
    if (fileDesc < 0 || fileDesc > 9) //FD's from 0->9
    {
        printf("Error: Invalid file descriptor!\n");
        return -1;
    }
    if ((running->fd[fileDesc]->mode != 0) &&
        (running->fd[fileDesc]->mode != 2)) //check mode for R and RW
    {
        printf("Error: File is not in a read ready mode!\n");
        return -1;
    }

    //skip 2 & 3
    //step 4
    if ((running->fd[fileDesc]->mptr->INODE.i_mode & 0xF000) != 0x8000)
    {
        printf("Error: Program can only read regular files!\n");
        return -1;
    }

    return myread_file(fileDesc, buf, nbytes, space);
}

int myread_file(int fileDesc, char *buf, long nbytes, int space)
{
    long count = 0, available, lbk, start, blk, remain;
    char kbuf[BLKSIZE], *cp;

    available = running->fd[fileDesc]->mptr->INODE.i_size - running->fd[fileDesc]->offset; //file size - offset

    if (nbytes > 8192)
        nbytes = 8192;

    if (nbytes > available)
        nbytes = available;

    while (nbytes > 0 && available > 0)
    {

        lbk = running->fd[fileDesc]->offset / BLKSIZE;
        start = running->fd[fileDesc]->offset % BLKSIZE;
        blk = map(running->fd[fileDesc]->mptr->INODE, lbk, running->fd[fileDesc]->mptr->dev);

        if (blk < 1 || blk > 1440)
        {
            //printf("block number = %d\n", blk);
            blk = 0;
        }
        //printf("block number = %d\n", blk);

        get_block(running->fd[fileDesc]->mptr->dev, blk, kbuf);
        cp = kbuf + start;
        remain = BLKSIZE - start;

        while (remain)
        {
            *buf++ = *cp++;
            running->fd[fileDesc]->offset++;
            count++;
            remain--;
            available--;
            nbytes--;
            if (nbytes <= 0 || available <= 0)
                break;
        }
    }

    return count;
}