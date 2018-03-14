#ifndef TYPE_H
#define TYPE_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>

//#include "global.c"  


/*************** type.h file ******************/
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc GD;
typedef struct ext2_inode INODE;
typedef struct ext2_dir_entry_2 DIR;

#define BLKSIZE 1024

#define NMINODE 100
#define NFD 16
#define NPROC 2
#define INODE_START 10

typedef struct minode
{
  INODE INODE;
  int dev, ino;
  int refCount;
  int dirty;
  int mounted;
  struct mntTable *mptr;
} MINODE;

typedef struct oft
{
  int mode;
  int refCount;
  MINODE *mptr;
  long offset;
} OFT;

typedef struct proc
{
  struct proc *next;
  int pid;
  int uid;
  MINODE *cwd;
  OFT *fd[NFD];
} PROC;

struct mntTable
{
  int dev;     // dev number: 0=FREE
  int nblock;  // s_blocks_count
  int ninodes; // s_inodes_count
  int bmap;    // bmap block#
  int imap;    // imap block#
  int iblock;  // inodes start block#
  MINODE *mountDirPtr;
  char deviceName[64];
  char mountedDirName[64];
} MTABLE;




extern MINODE *minode[NMINODE];
extern MINODE *root;
extern PROC *proc[NPROC], *running;

extern struct mntTable mtable[4];
//OFT oft[NOFT];

extern SUPER *sp;
extern GD *gp;
extern INODE *ip;

//extern int fd;                     //aka dev, but dev used elsewhere
extern int nblocks;                // from superblock
extern int ninodes;                // from superblock
extern int bmap;                   // bmap block
extern int imap;                   // imap block
extern int iblock;                 // inodes begin block
extern char *device;    //default device name
extern char gline[25], *gname[16]; // tokenized component string strings (g for global)
extern int nname;                  // number of component strings

//UTIL
long get_block(int dev, long blk, char buf[BLKSIZE]);
long put_block(int dev, long blk, char buf[BLKSIZE]);
int iput(MINODE *mip);
MINODE *iget(int dev, int ino);
void init();
int search(MINODE *mip, char *name);
int getino(int *deviceFD, char *pathname);
int tokenize(char *string, char **name[64]); //change later
void mount_root(char *diskname);
int tst_bit(char *buf, int bit);
int set_bit(char *buf, int bit);
int decFreeInodes(int dev);
long ialloc(int dev);
int decFreeBlocks(int dev);
long balloc(int dev);
int clr_bit(char *buf, int bit);
int incFreeInodes(int dev);
long idalloc(int dev, long ino);
long bdalloc(int dev, long bno);
int incFreeBlocks(int dev);
long map(INODE nInode, long lbk, int fileDesc);
void mytruncate(MINODE *mip);
int getFileDesc(char *pathname);
int permissionCheck(MINODE *mip);



//LEVEL 1
int ls_dir(char *pathname); //
void ls_file(MINODE *mip);
int mycd(char *pathname); //
void mypwd(MINODE *wd);
void rmypwd(MINODE *wd);
int mymkdir(char *pathname); //
int enter_child(MINODE *pip, int ino, char *child);
int quit();
void myclear();
int myrmdir(char *pathname); //
int mycreat(char *pathname); //
int enter_child_f(MINODE *pip, int ino, char *child);
int rm_child(MINODE *pmip, char *name);
void mylink(char *old_file, char *new_file);
int myunlink(char *pathname); //
void mysymlink(char *old_file, char *new_file);
int myreadlink(char *filename, char **buffer);
void mychmod(char *filename, char *chmode);
int mytouch(char *pathname); //
int mystat(char *pathname);


//LEVEL 2
int open_from_cmd(char *path, char *ppath);
int myopen(char *filename, int flags); //needs 2 params from cmd
int close_from_cmd(char *pathname);
int myclose(int fileDesc); //needs 1
int read_from_cmd(char *path, char *ppath);
int myk_read(int fileDesc, char buf[], long nbytes, int space); //needs 2
int myread_file(int fileDesc, char *buf, long nbytes, int space);
long lseek_from_cmd(char *path, char *ppath);
long mylseek(int fileDesc, long position); //needs 2
int write_from_cmd(char *path, char *ppath, char *pppath);
int myk_write(int fileDesc, char buf[], long nbytes, int space); //needs 3
int mywrite_file(int fileDesc, char *buf, long nbytes, int space);
int mycp(char *source, char *dest); //needs 2
int mymv(char *source, char *dest); //needs 2
int mycat(char *fildDesc);


//LEVEL 3
int mymount(char *path, char *ppath);


//MAIN HELPERS
int findCmd(char *command);
int doCmd(char *command, char *path, char *ppath, char *pppath);









#endif