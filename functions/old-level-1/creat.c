#include "../type.h"

int mycreat(char *pathname)
{
    char *temp1, *temp2, *parent, *child, buf[BLKSIZE];
    int pino, ino, blk, i;
    MINODE *pmip, *mip;
    DIR *dp;
    int fd = root->dev;

    temp1 = strdup(pathname); //break down into parent dir and new dir
    parent = dirname(temp1);
    temp2 = strdup(pathname);
    child = basename(temp2);

    printf("pathname = %s basename = %s\n", parent, child);

    pino = getino(&fd, parent); //find ino of parent
    pmip = iget(fd, pino); //get parent node

    if (pmip->INODE.i_mode != 0x41ED) //check specific directory, later on when user mode
    {                                 // is added we'll need to change this to check first bit for 0x4000
        printf("Error: \"%s\" is not a directory!\n", parent);
        return -1;
    }

    if (search(pmip, child) != 0) //if the child exist we can't make it
    {
        printf("Error: file \"%s\" already exist!\n", child);
        return -1;
    }

    ino = ialloc(pmip->dev);
    mip = iget(pmip->dev, ino);

    for (i = 0; i < 15; i++)
        mip->INODE.i_block[i] = 0;

    ip = &(mip->INODE);
    mip->dirty = 1;
    mip->ino = ino;
    ip->i_mode = 0x81a4;
    ip->i_uid = running->uid;
    ip->i_gid = running->cwd->INODE.i_gid;
    ip->i_size = 0;
    ip->i_links_count = 1;
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);
    ip->i_blocks = 0; // LINUX: Blocks count in 512-byte chunks
    iput(mip);

    enter_child_f(pmip, ino, child);
    pmip->dirty = 1; // mark minode dirty
    iput(pmip);      // write INODE to disk

    return 0;
}

int enter_child_f(MINODE *pip, int ino, char *child)
{
    int i = 0, ideal_length = 0, need_length = 0, remain = 0, temp_length = 0, blk, n;
    char buf[BLKSIZE], *cp, buf2[BLKSIZE];
    DIR *dp;
    MINODE *mip;

    //memset(buf, 0, BLKSIZE);

    need_length = 4 * ((8 + strlen(child) + 3) / 4);

    for (i = 0; i < 12; i++)
    {
        if (pip->INODE.i_block[i] == 0)
        {
            pip->INODE.i_size += BLKSIZE;
            dp->rec_len = BLKSIZE;
            dp->name_len = strlen(child);
            strncpy(dp->name, child, strlen(child));
            break;
        }
        else
        {
            get_block(pip->dev, pip->INODE.i_block[i], buf);
            dp = (DIR *)buf;
            cp = buf;

            while (cp + dp->rec_len < buf + BLKSIZE)
            {
                cp += dp->rec_len;
                dp = (DIR *)cp;
            }

            temp_length = dp->rec_len;
            ideal_length = 4 * ((8 + dp->name_len + 3) / 4);
            remain = dp->rec_len - ideal_length;

            if (remain >= need_length)
            {
                dp->rec_len = ideal_length;
                cp += ideal_length;
                temp_length -= ideal_length;
                dp = (DIR *)cp;
                dp->rec_len = temp_length;
                dp->name_len = strlen(child);
                dp->inode = ino;
                strncpy(dp->name, child, dp->name_len);
                break;
            }
            else
            {
                pip->INODE.i_blocks += 2;
                pip->INODE.i_size += BLKSIZE;
                blk = balloc(pip->dev);
                dp->rec_len = BLKSIZE;
                dp->name_len = strlen(child);
                strncpy(dp->name, child, strlen(child));
                pip->INODE.i_block[i] = blk;
                break;
            }
        }
    }

    put_block(pip->dev, pip->INODE.i_block[i], buf);
    pip->INODE.i_atime = time(0L);
}