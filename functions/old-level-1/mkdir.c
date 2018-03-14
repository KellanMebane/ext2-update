#include "../type.h"

int mymkdir(char *pathname)
{
    char *temp1, *temp2, *parent, *child, buf[BLKSIZE];
    int pino, ino, blk, fd = root->dev;
    MINODE *pmip, *mip;
    DIR *dp;

    temp1 = strdup(pathname); //break down into parent dir and new dir
    parent = dirname(temp1);
    temp2 = strdup(pathname);
    child = basename(temp2);

    printf("pathname = %s basename = %s\n", parent, child);

    if (strcmp(child, "/") == 0)
    {
        printf("Error: \"/\" already exist!\n");
        return -1;
    }

    pino = getino(&fd, parent); //find ino of parent
    pmip = iget(fd, pino); //get parent node

    if (pmip->INODE.i_mode != 0x41ED) //check specific directory, later on when user mode
    {                                 // is added we'll need to change this to check first bit for 0x4000
        printf("Error: \"%s\" is not a directory!\n", parent);
        return -1;
    }

    if (search(pmip, child) != 0) //if the child exist we can't make it
    {
        printf("Error: directory \"%s\" already exist!\n", child);
        return -1;
    }

    blk = balloc(fd);
    ino = ialloc(fd);

    mip = iget(fd, ino);
    ip = &(mip->INODE);
    ip->i_mode = 0x41ED;                   // 040755: DIR type and permissions
    ip->i_uid = running->uid; // owner uid
    ip->i_gid = running->cwd->INODE.i_gid; // group Id
    ip->i_size = BLKSIZE;                  // size in bytes
    ip->i_links_count = 2;                 // links count=2 because of . and ..
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);
    ip->i_blocks = 2;     // LINUX: Blocks count in 512-byte chunks
    ip->i_block[0] = blk; // new DIR has one data block
    for (int i = 1; i < 15; i++)
        ip->i_block[i] = 0;
    mip->dirty = 1; // mark minode dirty
    iput(mip);      // write INODE to disk

    //bzero(buf, BLKSZIZE); // optional: clear buf[ ] to 0
    memset(buf, 0, BLKSIZE);
    dp = (DIR *)buf;
    // make . entry
    dp->inode = ino;
    dp->rec_len = 12;
    dp->name_len = 1;
    dp->name[0] = '.';
    // make .. entry: pino=parent DIR ino, blk=allocated block
    dp = (char *)dp + 12;
    dp->inode = pino;
    dp->rec_len = BLKSIZE - 12; // rec_len spans block
    dp->name_len = 2;
    dp->name[0] = dp->name[1] = '.';
    put_block(fd, blk, buf); // write to blk on diks

    enter_child(pmip, ino, child);

    //running->cwd = mip;

    return 0;
}

int enter_child(MINODE *pip, int ino, char *child)
{
    int i = 0, ideal_length = 0, need_length = 0, remain = 0, temp_length = 0, blk, n;
    char buf[BLKSIZE], *cp, buf2[BLKSIZE];
    DIR *dp;

    need_length = 4 * ((8 + strlen(child) + 3) / 4);

    for (i = 0; i < 12; i++)
    {
        if (pip->INODE.i_block[i] == 0)
        {
            //pip->INODE.i_blocks += 2;
            pip->INODE.i_size += BLKSIZE;
            //blk = balloc(pip->dev);
            dp->rec_len = BLKSIZE;
            dp->name_len = strlen(child);
            strncpy(dp->name, child, strlen(child));
            //put_block(pip->dev, pip->INODE.i_block[i], buf);
            break;
        }
        else
        {
            get_block(pip->dev, pip->INODE.i_block[i], buf);
            dp = (DIR *)buf;
            cp = buf;

            while (cp + dp->rec_len < buf + BLKSIZE)
            {
                //printf("%d\n", dp->rec_len);
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
                //put_block(pip->dev, blk, buf);
                pip->INODE.i_block[i] = blk;
                break;
            }
        }
    }

    pip->INODE.i_links_count++;
    put_block(pip->dev, pip->INODE.i_block[i], buf);
    pip->INODE.i_atime = time(0L);

    return 1;
}