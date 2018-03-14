#include "../type.h"

void mypwd(MINODE *wd)
{
    //printf("root=%d wd=%d\n", root, wd);

    if (wd == root)
        printf("/");
    else
        rmypwd(wd);
}

void rmypwd(MINODE *wd)
{
    char buf[BLKSIZE], *cp, name[256];
    DIR *dp;
    int my_ino, parent_ino;
    MINODE *pip, *me, *temp;

    if (wd == root)
    {
        //printf("/");
        return;
    }

    if (wd->mounted == 1)
    {
        if (wd != root)
        {
            temp = wd;
            wd = temp->mptr->mountDirPtr; //when i get back, run debug on this. check if temp->mptr->dev = 3
        }
    }

    my_ino = search(wd, ".");
    parent_ino = search(wd, "..");

    pip = iget(wd->dev, parent_ino);                //get pointer to parent
    get_block(pip->dev, pip->INODE.i_block[0], buf); //get parent's block
    dp = (DIR *)buf;                           //typecast it as directory

    cp = buf;

    while (cp < buf + BLKSIZE) //search until you find yourself (raw thoughts)
    {
        if (dp->inode == my_ino) //found myself in my parents family tree
        {
            strcpy(name, dp->name); //copy name
            name[strlen(name)] = 0; //null terminate it
            break;
        }
        cp += dp->rec_len;
        dp = (DIR *)cp;
    }

    rmypwd(pip);
    printf("/%s", name);
}