#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <time.h>
// typedef u8, u16, u32 SUPER for convenience
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef struct ext2_group_desc GROUP;
GROUP *sp;
char buf[1024];
int fd;
int print(char *s, u32 x)
{
    printf("%-30s = %8d\n", s, x);
}
int group(char *device)
{
    fd = open(device, O_RDONLY);
    if (fd < 0)
    {
        printf("open %sfailed\n", device);
        exit(1);
    }
    lseek(fd, (long)1024 * 2, 0); // block 2, offset 2048
    read(fd, buf, 1024);
    sp = (GROUP *)buf; // as a group block structure

    print("bg_block_bitmap", sp->bg_block_bitmap);
    print("bg_inode_bitmap", sp->bg_inode_bitmap);
    print("bg_inode_table", sp->bg_inode_table);
    print("bg_free_blocks_count", sp->bg_free_blocks_count);
    print("bg_free_inodes_count", sp->bg_free_inodes_count);
    print("bg_used_dirs_count", sp->bg_used_dirs_count);
}
char *device = "mydisk"; // default device name
int main(int argc, char *argv[])
{
    if (argc > 1)
        device = argv[1];
    group(device);
}