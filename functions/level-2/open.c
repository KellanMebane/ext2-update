#include "../type.h"



//need pre open

int open_from_cmd(char *path, char *ppath)
{
    int flag;
    if (ppath[0] != 0)
        sscanf(ppath, "%d", &flag);
    else
        flag = 0; 
    myopen(path, flag);
}


int myopen(char *filename, int flags) //UNFINISHED, BUT GOOD ENOUGH FOR CAT TO WORK
{
    int ino, i, fd = root->dev;
    MINODE *mip;
    OFT *oftp;


    //step 1
    ino = getino(&fd, filename);
    if (ino == 0 && (flags == 2 || flags == 3 || flags == 1))
    {
        fd = root->dev;
        mycreat(filename);
        ino = getino(&fd, filename);
    }
    mip = iget(fd, ino);

    if (permissionCheck(mip))
        return -1;

    //step 2
    if ((mip->INODE.i_mode & 0xF000) != 0x8000)
    {
        printf("Error: \"%s\" not a regular file!\n", filename);
        iput(mip);
        return -1;
    }
    for (i = 0; i < 10; i++) //check for incompatible open modes
    {
        if (running->fd[i] != 0)
        {
            if (running->fd[i]->mptr == mip)
            {
                if (running->fd[i]->mode > 0)
                {
                    printf("Error: File \"%s\" already opened for writing\n", filename);
                    iput(mip);
                    return -1;
                }
            }
        }
    }

    //step 3
    oftp = malloc(sizeof(OFT)); //allocate an openTable entry

    //step 4
    i = 0;
    for (i = 0; i < 10; i++)
    {
        if (running->fd[i] == NULL)
            break;
    }
    if (i == 10)
    {
        printf("Error: not enough room to open file!\n");
        iput(mip);
        return -1;
    }
    running->fd[i] = oftp; //i is now the file descriptor

    oftp->mode = flags; //0=R 1=W 2=RW 3=APPEND
    oftp->refCount = 1;
    oftp->mptr = mip;
    switch (flags)
    {
    case 0:
        oftp->offset = 0;
        break;
    case 1:
        mytruncate(mip); // DO TRUNCATE IN UTIL.C
        oftp->offset = 0;
        break;
    case 2:
        oftp->offset = 0; // RW does NOT truncate file
        break;
    case 3:
        oftp->offset = mip->INODE.i_size; // APPEND mode
        break;
    default:
        printf("Error: mode \"%d\" is invalid\n", flags);
        return (-1);
    }
    //step 5
    mip->INODE.i_atime = mip->INODE.i_mtime = time(0L);
    mip->dirty = 1;
    //free(oftp);
    //iput(mip);

    return i;
}