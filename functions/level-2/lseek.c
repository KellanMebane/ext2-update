#include "../type.h"


//need pre lseek

long lseek_from_cmd(char *path, char *ppath)
{
    int index;
    long position;
    sscanf(ppath, "%ld", &position);

    if (ppath[0] == 0)
        position = 0;

    index = getFileDesc(path);
    if (index == -1)
    {
        printf("Error: Cannot seek a file that is not open!\n");
        return -1;
    }

    return mylseek(index, position);
}

long mylseek(int fileDesc, long position)
{
    long size = running->fd[fileDesc]->mptr->INODE.i_size;
    long original_position = running->fd[fileDesc]->offset;

    printf("Seeking: current position = %d\nSeeking: trying new position = %d\n", original_position, position);

    if (position > 0 && position < size)
        running->fd[fileDesc]->offset = position;
    else 
    {
        printf("Error: position not in file scope!\n");
        return -1;
    }

    printf("Seeking: now at position = %d\n", running->fd[fileDesc]->offset);

    return original_position;
}