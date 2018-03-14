#include "functions/type.h"

char *cmd[] = {"mount_root" /*0*/, "mkdir" /*1*/, "rmdir" /*2*/, "ls" /*3*/, "cd" /*4*/,
"pwd" /*5*/, "creat" /*6*/, "rm" /*7*/, "link" /*8*/, "unlink" /*9*/, "symlink" /*10*/,
"readlink" /*11*/, "stat" /*12*/, "chmod" /*13*/, "exit" /*14*/,
"touch" /*15*/, "open" /*16*/, "close" /*17*/,
"read" /*18*/, "write" /*19*/, "lseek" /*20*/, "cat" /*21*/, "cp" /*22*/, "mv" /*23*/,
"mount" /*24*/, "umount" /*25*/, "sudo" /*26*/, "unsudo"};

int findCmd(char *command)
{
    int i = 0;
    while (cmd[i])
    {
        if (strcmp(command, cmd[i]) == 0)
            return i; //command is found
        i++;
    }
    return -1; //we didn't find the command (invalid)
}


int doCmd(char *command, char *path, char *ppath, char *pppath)
{
    int function_to_call, res;
    char *buffer;

    function_to_call = findCmd(command);

    switch (function_to_call)
    {
        case 0: break;
        case 1: mymkdir(path); break;
        case 2: myrmdir(path); break;
        case 3: ls_dir(path); break;
        case 4: mycd(path); break;
        case 5: mypwd(running->cwd); printf("\n"); break;
        case 6: mycreat(path); break;
        case 7: myunlink(path); break;
        case 8: mylink(path, ppath); break;
        case 9: myunlink(path); break;
        case 10: mysymlink(path, ppath); break;
        case 11: res = myreadlink(path, &buffer); printf("name = %s size = %d\n", buffer, res); break;
        case 12: mystat(path); break;
        case 13: mychmod(ppath, path); break;
        case 14: quit(); break;
        case 15: mytouch(path); break;
        case 16: open_from_cmd(path, ppath); break;
        case 17: close_from_cmd(path); break;
        case 18: read_from_cmd(path, ppath); break;
        case 19: write_from_cmd(path, ppath, pppath); break;
        case 20: lseek_from_cmd(path, ppath); break;
        case 21: mycat(path); break;
        case 22: mycp(path, ppath); break;
        case 23: mymv(path, ppath); break;
        case 24: printf("doing mount\n"); mymount(path, ppath); break;
        case 25: /*UMOUNT*/ break;
        case 26: printf("Invalid command, please try again.\n"); break;
        case 27: proc[0]->cwd = proc[1]->cwd; running = proc[0]; break;
        case 28: proc[1]->cwd = proc[0]->cwd; running = proc[1]; break;
    }
}