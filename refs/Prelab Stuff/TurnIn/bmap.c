/*************** imap.c program **************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc GD;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
#define BLKSIZE 1024

SUPER *sp;
GD *gp;
char buf[BLKSIZE];
int fd;


// reversed order of this algorithm thanks to https://stackoverflow.com/a/3974138
// and KC's "test" algorithm
void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    for (i=size-1;i>=0;i--)
    {
        for (j=0;j<=7;j++)
        {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
    printf(" ");
}



// get_block() reads a disk block into a buf[ ]
int get_block(int fd, int blk, char *buf)
{
    lseek(fd, (long)blk * BLKSIZE, SEEK_SET);
    return read(fd, buf, BLKSIZE);
}
int bmap(char *device)
{
    int i, nbnodes, blksize, bmapblk, j;
    u8 *b, byte;
    fd = open(device, O_RDONLY);
    if (fd < 0)
    {
        printf("open %s failed\n", device);
        exit(1);
    }
    get_block(fd, 1, buf); // get superblock
    sp = (SUPER *)buf;
    // check magic number to ensure it’s an EXT2 FS
    nbnodes = sp->s_blocks_count; // get blocks_count
    printf("nbnodes = %d\n", nbnodes);
    get_block(fd, 2, buf); // get group descriptor
    gp = (GD *)buf;
    bmapblk = gp->bg_block_bitmap; // get imap block number
    printf("bmapblk = %d\n", bmapblk);
    get_block(fd, bmapblk, buf); // get imap block into buf[ ]
    for (i = 0; i <= nbnodes / 8; i++)
    { // print each byte in HEX
        printf("% 02x ", (u8)buf[i]);
    }
    printf("\n");

    //buf[i] = hex number, print that number in binary reversed
    for (i = 0; i <= nbnodes / 8; i++)
        printBits(sizeof((u8)buf[i]), &buf[i]);

    printf("\n");
}
char *dev = "mydisk"; // default device
int main(int argc, char *argv[])
{
    if (argc > 1)
        dev = argv[1];
    bmap(dev);
}