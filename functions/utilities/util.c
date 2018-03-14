#include "../type.h"

long get_block(int dev, long blk, char buf[BLKSIZE])
{
    lseek(dev, (long)(blk * BLKSIZE), 0);
    read(dev, buf, BLKSIZE);
}

long put_block(int dev, long blk, char buf[BLKSIZE])
{
    lseek(dev, (long)(blk * BLKSIZE), 0);
    write(dev, buf, BLKSIZE);
}

//might not be correct, idk confusing notes.
void init()
{
    int i;

    //initialize proc1 and proc2
    proc[0] = malloc(sizeof(PROC));
    proc[0]->cwd = 0;
    proc[1] = malloc(sizeof(PROC));
    proc[1]->cwd = 0;
    proc[0]->uid = 0;
    proc[1]->uid = 1;
    proc[0]->pid = 1;
    proc[1]->pid = 2;

    //initialize minodes with refcount 0
    for (i = 0; i < 100; i++)
    {
        minode[i] = malloc(sizeof(MINODE));
        minode[i]->refCount = 0;
    }

    root = malloc(sizeof(MINODE));
}

MINODE *iget(int dev, int ino)
{
    int i = 0, blk, disp;
    char buf[BLKSIZE];
    MINODE *mip;

    //step 1
    while (i < 100)
    {
        if ((minode[i]->dev == dev) && (minode[i]->ino == ino))
        {
            mip = minode[i];
            mip->refCount++;
            //printf("In iget: return at step 1\n");
            return mip;
        }
        i++;
    }

    //printf("In iget: above step 2\n");
    //step 2
    i = 0;
    while (i < 100)
    {
        if (minode[i]->refCount == 0)
        {
            //printf("In iget: refCount == 0!\n");
            mip = minode[i];
            mip->refCount = 1;
            mip->dev = dev;
            mip->ino = ino;
            mip->dirty = 0;
            mip->mounted = 0;
            mip->mptr = 0;

            //step 3
            blk = (ino - 1) / 8 + INODE_START;
            disp = (ino - 1) % 8;
            get_block(dev, blk, buf);
            ip = (INODE *)buf + disp;
            mip->INODE = *ip;

            //step 4
            //printf("In iget: return at step 4\n");
            return mip;
        }
        i++;
    }

    return 0;
}

int iput(MINODE *mip)
{
    int blk, disp;
    char buf[BLKSIZE];

    //step 1
    mip->refCount--;

    //step 2
    if (mip->refCount > 0)
        return 0;
    if (!mip->dirty)
        return 0;

    //step 3
    //printf("iput: dev=%d ino=%d\n", mip->dev, mip->ino);
    blk = (mip->ino - 1) / 8 + INODE_START;
    disp = (mip->ino - 1) % 8;
    get_block(mip->dev, blk, buf);
    ip = (INODE *)buf + disp;
    *ip = mip->INODE;
    put_block(mip->dev, blk, buf);
}

int search(MINODE *mip, char *name)
{
    char buf[BLKSIZE];
    DIR *dp;
    char *cp, temp[BLKSIZE];

    get_block(mip->dev, mip->INODE.i_block[0], buf);

    cp = buf;
    dp = (DIR *)buf;

    //printf("In search...\n");
    while (cp < buf + BLKSIZE)
    {
        //printf("In search: while loop...\n");
        strncpy(temp, dp->name, dp->name_len);

        temp[dp->name_len] = 0;

        if (strcmp(name, temp) == 0)
        {
            //printf("In searching: found name inode=%d\n", dp->inode);
            return dp->inode;
        }
        else
        {
            cp += dp->rec_len;
            dp = (DIR *)cp;
        }
        memset(temp, 0, BLKSIZE);
    }

    //printf("In search: exit!\n");
    return 0;
}

int getino(int *deviceFD, char *pathname)
{
    int i, ino, blk, disp, n, newdev, j;
    char buf[BLKSIZE], *name[64], newname[256];
    INODE *ip;
    MINODE *mip, *temp;
    int nfd = *deviceFD;

    //printf("getino: pathname=%s\n", pathname);

    if (pathname[0] == 0)
        mip = iget(nfd, running->cwd->ino);
    //return 2;
    else
    {
        if (strcmp(pathname, "/") == 0)
            return 2;

        if (pathname[0] == '/')
            mip = iget(nfd, 2);
        else
            mip = iget(nfd, running->cwd->ino);
    }

    //printf("In getino: mip == root\n");
    //else if (mip == running->cwd)
    //printf("In getino: mip == cwd\n");

    strcpy(buf, pathname);
    n = tokenize(buf, &name); // n = number of token strings
    //printf("In getino: n=%d\n", n);
    for (i = 0; i <= n; i++)
    {
        //printf("===========================================\n");
        //printf("getino: i=%d name[%d]=%s\n", i, i, name[i]);

        ino = search(mip, name[i]);
        //printf("In getino: ino=%d\n", ino);
        if (ino == 0)
        {
            iput(mip);
            printf("name %s does not exist\n", name[i]);
            return 0;
        }
        iput(mip);

        mip = iget(nfd, ino);

        if (mip->mounted == 1) //check if mip is mounted
        {
            *deviceFD = nfd = mip->mptr->dev;
            ino = 2; //start at root of next device
            iput(mip);
            mip = iget(nfd, ino);
            printf("Swapping to mount point %d\n", nfd - 3); //calling ROOT 0 point (not including STDIO)
        }
    }
    iput(mip);

    return ino;
}

int tokenize(char *string, char **name[64]) //change later
{
    int n = 0;
    char *temp, *backup;
    backup = malloc(sizeof(char) * strlen(string));
    strcpy(backup, string);
    temp = strtok(backup, "/");
    while (temp != NULL)
    {
        name[n] = temp;
        name[n + 1] = 0;
        n++;
        //printf("In tokenize: n=%d,temp=%s\n", n, temp);
        temp = strtok(0, "/");
    }
    if (n != 0)
        n--;
    return n;
}

void mount_root(char *diskname)
{
    char buf[BLKSIZE];
    int fd;
    GD *gd;
    fd = open(diskname, O_RDWR);
    if (fd < 0)
    {
        printf("Failed to open disk=\"%s\"\n", diskname);
        exit(0);
    }
    root = iget(fd, 2);
    proc[0]->cwd = iget(fd, 2);
    proc[1]->cwd = iget(fd, 2);
    get_block(fd, 1, buf);
    sp = (SUPER *)buf;
    if (sp->s_magic != 0xEF53 && sp->s_magic != 0xEF51)
    {
        printf("Disk=\"%s\" is not a valid diskname", diskname);
        close(fd);
        exit(0);
    }

    mtable[0].dev = fd;
    mtable[0].nblock = sp->s_blocks_count;
    mtable[0].ninodes = sp->s_inodes_count;

    get_block(fd, 2, buf);
    gd = (GD *)buf;
    mtable[0].bmap = gd->bg_block_bitmap;
    bmap = mtable[0].bmap;
    mtable[0].imap = gd->bg_inode_bitmap;
    imap = mtable[0].imap;
    mtable[0].iblock = gd->bg_inode_table;
    mtable[0].mountDirPtr = root;
    strcpy(mtable[0].deviceName, device);
    strcpy(mtable[0].mountedDirName, "/");
    running = proc[0];
    root->mptr = &mtable[0];
    root->mounted = 1;
}

// tst_bit, set_bit functions
int tst_bit(char *buf, int bit)
{
    return buf[bit / 8] & (1 << (bit % 8));
}

int set_bit(char *buf, int bit)
{
    buf[bit / 8] |= (1 << (bit % 8));
}

int decFreeInodes(int dev)
{
    char buf[BLKSIZE];
    // dec free inodes count in SUPER and GD
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_inodes_count--;
    put_block(dev, 1, buf);
    get_block(dev, 2, buf);
    gp = (GD *)buf;
    gp->bg_free_inodes_count--;
    put_block(dev, 2, buf);
}

long ialloc(int dev)
{
    int i;
    char buf[BLKSIZE];
    // assume imap, bmap are globals from superblock and GD
    get_block(dev, imap, buf);
    for (i = 0; i < mtable[dev - 3].ninodes; i++)
    {
        if (tst_bit(buf, i) == 0)
        {
            set_bit(buf, i);
            put_block(dev, imap, buf);
            // update free inode count in SUPER and GD
            decFreeInodes(dev);
            return (i + 1);
        }
    }
    return 0; // out of FREE inodes
}

int decFreeBlocks(int dev)
{
    char buf[BLKSIZE];
    // dec free inodes count in SUPER and GD
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_blocks_count--;
    put_block(dev, 1, buf);
    get_block(dev, 2, buf);
    gp = (GD *)buf;
    gp->bg_free_blocks_count--;
    put_block(dev, 2, buf);
}

long balloc(int dev)
{
    int i;
    char buf[BLKSIZE];
    // assume imap, bmap are globals from superblock and GD
    get_block(dev, bmap, buf);
    for (i = 0; i < mtable[dev - 3].nblock; i++)
    {
        if (tst_bit(buf, i) == 0)
        {
            set_bit(buf, i);
            put_block(dev, bmap, buf);
            // update free block count in SUPER and GD
            decFreeBlocks(dev);
            return (i + 1);
        }
    }
    return 0; // out of FREE blocks
}

int clr_bit(char *buf, int bit) // clear bit in char buf[BLKSIZE]
{
    buf[bit / 8] &= ~(1 << (bit % 8));
}

int incFreeInodes(int dev)
{
    char buf[BLKSIZE];
    // inc free inodes count in SUPER and GD
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_inodes_count++;
    put_block(dev, 1, buf);
    get_block(dev, 2, buf);
    gp = (GD *)buf;
    gp->bg_free_inodes_count++;
    put_block(dev, 2, buf);
}
long idalloc(int dev, long ino)
{
    int i;
    char buf[BLKSIZE];
    if (ino > mtable[dev - 3].ninodes)
    { // niodes global
        printf("inumber %d out of range\n", ino);
        return;
    }
    // get inode bitmap block
    get_block(dev, imap, buf);
    clr_bit(buf, ino - 1);
    // write buf back
    put_block(dev, imap, buf);
    // update free inode count in SUPER and GD
    incFreeInodes(dev);
}
long bdalloc(int dev, long bno)
{
    int i;
    char buf[BLKSIZE];
    if (bno > mtable[dev - 3].nblock)
    { // nblocks global
        printf("bnumber %d out of range\n", bno);
        return;
    }
    // get inode bitmap block
    get_block(dev, bmap, buf);
    clr_bit(buf, bno - 1);
    // write buf back
    put_block(dev, bmap, buf);
    // update free inode count in SUPER and GD
    incFreeBlocks(dev);
}
int incFreeBlocks(int dev)
{
    char buf[BLKSIZE];
    // inc free inodes count in SUPER and GD
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_blocks_count++;
    put_block(dev, 1, buf);
    get_block(dev, 2, buf);
    gp = (GD *)buf;
    gp->bg_free_blocks_count++;
    put_block(dev, 2, buf);
}

long map(INODE nInode, long lbk, int fileDesc)
{
    long blk = -3, dblk;
    u32 ibuf[256], dbuf[256];

    if (lbk < 12) //direct
        blk = nInode.i_block[lbk];
    else if (12 <= lbk && lbk < 12 + 256) //indirect
    {
        get_block(fileDesc, nInode.i_block[12], ibuf);
        blk = ibuf[lbk - 12];
    }
    else //double indirect
    {
        get_block(fileDesc, nInode.i_block[13], ibuf);
        lbk -= (12 + 256);
        dblk = ibuf[lbk / 256];
        get_block(fileDesc, dblk, dbuf);
        blk = dbuf[lbk % 256];
    }

    return blk;
}

void mytruncate(MINODE *mip) //finish (kellan)
{
    long size, offset = 0, available, remain, blk, lbk, count = 0, start;
    char emptyBlock[1024];
    size = mip->INODE.i_size;
    available = size - offset;
    memset(emptyBlock, 0, 1024);

    //printf("size = %d\n", size);

    while (size)
    {
        lbk = offset / BLKSIZE;
        start = offset % BLKSIZE;
        blk = map(mip->INODE, lbk, mip->dev);
        if (blk < 12 || blk > 1474559)
            break;

        put_block(mip->dev, blk, emptyBlock);
        //printf("removing block = %d\n", blk);
        bdalloc(mip->dev, blk);
        remain = BLKSIZE - start;
        while (remain)
        {
            offset++;
            count++;
            remain--;
            available--;
            size--;
            if (size == 0 || available == 0)
                break;
        }
    }

    mip->INODE.i_size = 0;
    minode[mip->ino]->dirty = 1;
    mip->dirty = 1;
}

int getFileDesc(char *pathname)
{
    int ino, index, yes = 0, fd = root->dev;
    MINODE *mip;

    ino = getino(&fd, pathname);
    if (ino < 2)
    {
        printf("Error: \"%s\" not on disk!\n", pathname);
        return -1;
    }
    mip = iget(fd, ino);

    if ((mip->INODE.i_mode & 0xF000) != 0x8000)
    {
        printf("Error: \"%s\" not a regular file!\n", pathname);
        iput(mip);
        return -1;
    }

    for (index = 0; index < 10; index++)
    {
        if (running->fd[index] != 0)
        {
            if (running->fd[index]->mptr == mip)
            {
                yes = 1;
                break;
            }
        }
    }

    if (yes)
        return index;
    else
        return -1;
}

int permissionCheck(MINODE *mip)
{
    if (running->uid == 0 || mip->INODE.i_uid == running->uid) //super user, or uid match
    {
        return 0;
    }
    else if (mip->INODE.i_uid != running->uid)
    {
        printf("Do not have permission because not owner");
        return -1;
    }
}