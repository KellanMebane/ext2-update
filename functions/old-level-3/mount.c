#include "../type.h"

int mymount(char *path, char *ppath)
{
    int index, md, ino, fd = root->dev;
    MINODE *mip;
    GD *gd;
    SUPER *sb;

    char buf[BLKSIZE];

    if (path[0] == 0) //show mounts call
    {
        for (index = 0; index < 4; index++)
        {
            if (mtable[index].dev != 0)
            {
                printf("%d: %s mounted on %s\n", mtable[index].dev - 3,
                       mtable[index].deviceName, mtable[index].mountedDirName);
                return 0;
            }
        }
    }

    if (strcmp(ppath, "") == 0) //invalid mount call
    {
        printf("Format: mount [source] [destination]\n");
        return -1;
    }

    for (index = 0; index < 4; index++) //check to see if mounted by name
    {
        if (strcmp(mtable[index].mountedDirName, path) == 0)
        {
            printf("Error: Device \"%s\" already mounted!\n", path);
            return -1;
        }
    }

    md = open(path, O_RDWR);
    if (md < 0)
    {
        printf("Failed to open disk=\"%s\"\n", path);
        return -1;
    }
    get_block(md, 1, buf);
    sb = (SUPER *)buf;
    if (sb->s_magic != 0xEF53 && sb->s_magic != 0xEF51) //check if EXT2
    {
        printf("Disk=\"%s\" is not a valid diskname", path);
        close(md);
        return -1;
    }

    ino = getino(&fd, ppath);
    mip = iget(fd, ino);

    if (mip->INODE.i_mode != 0x41ED) //check specific directory, later on when user mode
    {                                // is added we'll need to change this to
                                     //check first bit for 0x4000
        printf("Error: \"%s\" is not a directory!\n", ppath);
        return -1;
    }

    if (mip->refCount > 1)
    {
        printf("Error: \"%s\" is busy!\n", ppath);
        return -1;
    }

    mip->mounted = 1;
    mip->mptr = &mtable[md - 3];

    mtable[md - 3].dev = md;
    mtable[md - 3].nblock = sb->s_blocks_count;
    mtable[md - 3].ninodes = sb->s_inodes_count;

    get_block(md, 2, buf);
    gd = (GD *)buf;
    mtable[md - 3].bmap = gd->bg_block_bitmap;
    mtable[md - 3].imap = gd->bg_inode_bitmap;
    mtable[md - 3].iblock = gd->bg_inode_table;
    mtable[md - 3].mountDirPtr = mip;
    strcpy(mtable[md - 3].deviceName, path);
    strcpy(mtable[md - 3].mountedDirName, ppath);
    mip = iget(md, 2); //think this is how it works?
    mip->mptr = &mtable[md - 3];
    mip->mounted = 1;
    mip->mptr->dev = md;


    return 0;
}