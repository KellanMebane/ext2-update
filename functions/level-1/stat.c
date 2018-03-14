#include "../type.h"


// look at ls
int mystat(char *pathname)//, STAT *stPtr)
{
    int iblock, ino;
    int fd = root->dev;
    char buf[BLKSIZE], ftime[64];
    time_t curtime;
    MINODE *mip;

    if (pathname[0] != 0) //not null pathname
    {
        ino = getino(&fd, pathname); //get inode number

        if (ino < 2){
            iput(mip); //not an inode
            return;
        }

        mip = iget(fd, ino);

        if (permissionCheck(mip))
            return -1;

        if (ino == 0)
        {
            iput(mip);
            return 0;
        }

        printf("st_mode: %d\n", mip->INODE.i_mode);
        printf("st_nlink: %d\n", mip->INODE.i_links_count);
        printf("st_uid: %d\n", mip->INODE.i_uid);
        printf("st_gid: %d\n", mip->INODE.i_gid);
        printf("st_size: %d\n", mip->INODE.i_size);
        printf("st_blksize: 1024\n");
        printf("st_blocks: %d\n", mip->INODE.i_blocks);
        printf("st_atime: %d\n", mip->INODE.i_atime);
        printf("st_mtime: %d\n", mip->INODE.i_mtime);
        printf("st_ctime: %d\n", mip->INODE.i_ctime);
        curtime = (time_t)mip->INODE.i_ctime;
        strcpy(ftime, ctime(&curtime));
        ftime[strlen(ftime) - 1] = 0;
        printf("date = %s\n", ftime);
    }
    //printf("In inode: ino=%d\n", ino);

    //ls_file(mip);
    iput(mip);


    return 0;
}