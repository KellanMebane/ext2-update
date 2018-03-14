#include "../type.h"

//1024 instead of buf
// look at read 8196
//getfiledesc - converts path name to see if its an open file = -1 if closed
// nbytes -1025.
// myk_read, while (n > 0) {last 3 lines of read}

int mycat (char *fildDesc)
{
    int ino, n = 1024;
    int nfd;
    int fd = root->dev;
    char buf[BLKSIZE + 1];
    MINODE *mip;

    ino = getino (&fd, fildDesc);
    mip = iget(fd, ino);
    

    if (permissionCheck(mip))
    {
        iput(mip);
        return -1;
    }
    iput(mip);

    nfd = myopen (fildDesc, 0);
    while (n > 0)
    {
        n = myk_read (nfd, buf, 1024, 0);
        buf[n] = 0;
        printf("%s", buf);
    }
    
    printf("\n");
    myclose(nfd);
}