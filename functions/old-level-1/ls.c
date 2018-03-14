#include "../type.h"



int ls_dir(char *pathname) //LS FOR NOW
{
    int i, fd = root->dev;
    char *cp, temp[256], buf[BLKSIZE];
    int blk;
    DIR *dp;
    int iblock, ino;
    MINODE *mip;

    //GOING THROUGH DIRECTORY AND PRINTING ...EXERCISE 4...
    printf("-----------------------------\n");
    printf("Printing DIR Contents\n");
    printf("-----------------------------\n");

    if (pathname[0] != 0)
    {
        ino = getino(&fd, pathname);

        if (ino < 2)
            return;

        mip = iget(fd, ino);

        if (ino == 0)
        {
            iput(mip);
            return 0;
        }

        //printf("In inode: ino=%d\n", ino);
        get_block(mip->dev, mip->INODE.i_block[0], buf);
        ip = (INODE *)buf;
        iput(mip);
    }
    else
    {

        get_block(running->cwd->dev, running->cwd->INODE.i_block[0], buf);
        ip = (INODE *)buf;
    }

    dp = (DIR *)buf;
    cp = buf;

    while (cp < buf + BLKSIZE)
    {
        strncpy(temp, dp->name, dp->name_len);
        temp[dp->name_len] = 0;
        //ip = (INODE *)buf;

        mip = iget(fd, dp->inode);

        ls_file(mip);
        printf(" reclen = %d name = %s\n", dp->rec_len, temp);
        iput(mip);
        cp += dp->rec_len;
        dp = (DIR *)cp;
    }
    printf("end of ls\n");
}

void ls_file(MINODE *mip)
{
    //MINODE *mip = iget(fd, ino);
    char ftime[64], *temp;
    time_t curtime;
    int i;
    char *t1 = "xwrxwrxwr-------";
    char *t2 = "----------------";

    if ((mip->INODE.i_mode & 0xF000) == 0x8000)
        printf("%c", '-');
    if ((mip->INODE.i_mode & 0xF000) == 0x4000)
        printf("%c", 'd');
    if ((mip->INODE.i_mode & 0xF000) == 0xA000)
        printf("%c", 'l');

    for (i = 8; i >= 0; i--)
    {
        if (mip->INODE.i_mode & (1 << i))
            printf("%c", t1[i]);
        else
            printf("%c", t2[i]);
    }

    printf("%4d ", mip->INODE.i_links_count);
    printf("%4d ", mip->INODE.i_gid);
    printf("%4d ", mip->INODE.i_uid);
    printf("%8d ", mip->INODE.i_size);
    curtime = (time_t)mip->INODE.i_ctime;
    strcpy(ftime, ctime(&curtime));
    ftime[strlen(ftime) - 1] = 0;
    //ftime[strlen(ftime) - 2] = 0;
    //ftime[strlen(ftime) - 3] = 0;
    //ftime[strlen(ftime)-4] = 0;
    //ftime[strlen(ftime)-5] = 0;
    printf("date = %s", ftime);
    //iput(mip);
}