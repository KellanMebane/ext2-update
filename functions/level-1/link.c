#include "../type.h"

void mylink(char *old_file, char *new_file)
{
    int oino, pino, fd = root->dev;
    MINODE *omip, *pmip;
    char *temp1, *temp2, *parent, *child;

    temp1 = strdup(new_file); //break down into parent dir and new dir
    parent = dirname(temp1);
    temp2 = strdup(new_file);
    child = basename(temp2);

    //old location
    oino = getino(&fd, old_file); //get inode of existing file
    if (oino < 2)            //if it's less than root return
    {
        printf("path %s not on disk!\n", old_file);
        return;
    }

    omip = iget(fd, oino);

    if (permissionCheck(omip))
        return -1;

    if (omip->INODE.i_mode == 0x41ED) //check specific directory, later on when user mode
    {                                 // is added we'll need to change this to
                                      //check first bit for 0x4000
        printf("Error: \"%s\" is a directory, cannot link!\n", child);
        iput(omip);
        return;
    }

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

    //if here, just need to enter the inode in the new directory
    enter_child_f(pmip, oino, child);
    omip->INODE.i_links_count++; //increment links count
    omip->dirty = 1;             //flag dirty
    iput(omip);                  //put away gathered nodes
    iput(pmip);

    return;
}