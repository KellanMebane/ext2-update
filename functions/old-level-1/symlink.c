#include "../type.h"


void mysymlink(char *old_file, char *new_file)
{
    int oino, pino, ino, i, blk, fd = root->dev;
    MINODE *omip, *pmip, *mip;
    char *temp1, *temp2, *parent, *child, *temp3, *oldname;
    DIR *dp;
    char *cp, *buf;

    temp1 = strdup(new_file); //break down into parent dir and new dir
    parent = dirname(temp1);
    temp2 = strdup(new_file);
    child = basename(temp2);
    temp3 = strdup(old_file);
    oldname = basename(temp3);

    //old location
    oino = getino(&fd, old_file); //get inode of existing file
    if (oino < 2)            //if it's less than root return
    {
        printf("path %s not on disk!\n", old_file);
        return;
    }
    omip = iget(fd, oino);

    fd = root->dev;

    //new location
    pino = getino(&fd, parent);
    if (pino < 2)
    {
        printf("path \"%s\" not on disk!\n", parent);
        iput(omip);
        return;
    }

    pmip = iget(fd, pino);
    if (pmip->INODE.i_mode != 0x41ED) //check specific directory, later on when user mode
    {                                 // is added we'll need to change this to
                                      //check first bit for 0x4000
        printf("Error: \"%s\" is not a directory!\n", parent);
        iput(omip);
        iput(pmip);
        return;
    }

    if (search(pmip, child) != 0) //check to see if it exists already
    {
        printf("Error: file \"%s\" already exists!\n", child);
        iput(pmip); //put away gathered nodes
        iput(omip);
        return;
    }

    //we have source node, new child name, and parent of new child

    ino = ialloc(fd);
    blk = balloc(fd);
    mip = iget(fd, ino);

    for (i = 1; i < 15; i++)
        mip->INODE.i_block[i] = 0;

    mip->INODE.i_block[0] = blk;

    ip = &(mip->INODE);
    mip->dirty = 1;
    mip->ino = ino;
    ip->i_mode = 0xA1a4;
    ip->i_uid = running->cwd->INODE.i_uid;
    ip->i_gid = running->cwd->INODE.i_gid;
    ip->i_size = strlen(oldname);
    ip->i_links_count = 1;
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);
    ip->i_blocks = 2; // LINUX: Blocks count in 512-byte chunks

    //get_block(fd, mip->INODE.i_block[0], buf);
    dp = (DIR *)buf;
    strcpy(dp->name, oldname);
    dp->name_len = strlen(dp->name);
    dp->inode = oino;
    dp->rec_len = BLKSIZE;
    put_block(mip->dev, mip->INODE.i_block[0], buf);

    //enter_child(mip, oino, oldname);
    iput(mip);

    enter_child(pmip, ino, child);
    pmip->dirty = 1; // mark minode dirty
    iput(pmip);      // write INODE to disk
    iput(omip);
}