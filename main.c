//KELLAN MEBANE

#include "functions/type.h"

int main(int argc, char *argv[])
{
    char input[128], command[64], path[64], ppath[64], pppath[64];
    int n, fd, c = 0, i;
    MINODE *temp;
    char tmp[1025];

    init();

    if (argc > 1)
        device = argv[1];
    else
        device = "diskimage";

    mount_root(device);


    while (1)
    {
        printf("input a command [level one commands] : ");
        temp = running->cwd;
        mypwd(running->cwd);
        ;
        printf("$ ");
        fgets(input, 128, stdin);
        input[strlen(input) - 1] = 0;
        if (input[0] == 0)
            continue;

        path[0] = 0;
        n = sscanf(input, "%s %s %s %s", command, path, ppath, pppath);

        //printf("%d\n", n);

        if (n == 1)
        {
            strcpy(path, "");
            strcpy(ppath, "");
            strcpy(pppath, "");
        }
        if (n == 2)
        {
            strcpy(ppath, "");
            strcpy(pppath, "");
        }
        if (n == 3)
            strcpy(pppath, "");

        ////////////////////////////////////
        doCmd(command, path, ppath, pppath);
        ////////////////////////////////////

        memset(input, 0, 128);
        memset(command, 0, 64);
        memset(path, 0, 128);
        memset(ppath, 0, 128);
        memset(pppath, 0, 128);
    }
}