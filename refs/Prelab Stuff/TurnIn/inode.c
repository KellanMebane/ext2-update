/*********** inode.c file **********/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>
#define BLKSIZE 1024
typedef struct ext2_group_desc GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode INODE;
typedef struct ext2_dir_entry_2 DIR;
SUPER *sp;
GD *gp;
INODE *ip;
DIR *dp;
char buf[BLKSIZE];
int fd, firstdata, inodesize, blksize, iblock;
char *dev = "mydisk";
int get_block(int fd, int blk, char *buf)
{
    lseek(fd, blk * BLKSIZE, SEEK_SET);
    return read(fd, buf, BLKSIZE);
}
int inode(char *dev)
{
    int i;
    char *cp, temp[256];
    int blk;
    fd = open(dev, O_RDONLY);
    if (fd < 0)
    {
        printf("open failed\n");
        exit(1);
    }
    /*************************************
 same code as before to check EXT2 FS
 **************************************/
    get_block(fd, 2, buf); // get group descriptor
    gp = (GD *)buf;
    printf("bmap_block=%d imap_block=%d inodes_table=%d ",
    gp->bg_block_bitmap,
    gp->bg_inode_bitmap,
    gp->bg_inode_table,
    iblock = gp->bg_inode_table);
    printf("root inode information:\n");
    printf("-----------------------\n");
    get_block(fd, iblock, buf);
    ip = (INODE *)buf;
    ip++; // ip point at #2 INODE
    printf("mode = %4x ", ip->i_mode);
    printf("uid = %d gid = %d\n", ip->i_uid, ip->i_gid);
    printf("size = %d\n", ip->i_size);
    printf("ctime = %s", ctime(&ip->i_ctime));
    printf("links = %d\n", ip->i_links_count);
    for (i=0; i<15; i++){ // print disk block numbers
        if (ip->i_block[i]) // print non-zero blocks only
            printf("i_block[%d] = %d\n", i, ip->i_block[i]);
    }


    //GOING THROUGH DIRECTORY AND PRINTING ...EXERCISE 4...
    printf("-----------------------------\n");
    printf("Printing DIR Contents\n");
    printf("-----------------------------\n");

    blk = ip->i_block[0];
    get_block(fd, blk, buf);
    dp = (DIR *)buf;
    cp = buf;
    while  (cp < buf + BLKSIZE)
    {
        strncpy(temp, dp->name, dp->name_len);
        temp[dp->name_len] = 0;
        printf("%d %d %d %s\n", dp->inode, dp->rec_len, dp->name_len, temp);
        cp += dp->rec_len;
        dp = (DIR *)cp;
    }



















}
int main(int argc, char *argv[])
{
    if (argc > 1)
        dev = argv[1];
    inode(dev);
}