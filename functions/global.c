#ifndef GLOBAL_C
#define GLOBAL_C

#include "type.h"

MINODE *minode[NMINODE];
MINODE *root;
PROC *proc[NPROC], *running;

struct mntTable mtable[4];
//OFT oft[NOFT];

SUPER *sp;
GD *gp;
INODE *ip;

//int fd;                     //aka dev, but dev used elsewhere
int nblocks;                // from superblock
int ninodes;                // from superblock
int bmap;                   // bmap block
int imap;                   // imap block
int iblock;                 // inodes begin block
char *device = "mydisk";    //default device name
char gline[25], *gname[16]; // tokenized component string strings (g for global)
int nname;                  // number of component strings

#endif