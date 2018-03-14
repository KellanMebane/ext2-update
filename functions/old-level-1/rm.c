#include "../type.h"



int rm_child(MINODE *pmip, char *name)
{
    int i = 0, ideal_length = 0, need_length = 0, remain = 0, temp_length = 0, blk, n;
    char buf[BLKSIZE], *cp, buf2[BLKSIZE];
    DIR *dp, *ndp;
    int j = 0; //j substitutes i
    DIR *tdp;
    char *tcp, *ncp;
    int temp_rec_len = 0;

    for (i = 0; i < 12; i++) //cycle through direct blocks
    {
        if (pmip->INODE.i_block[i] == 0)
            return;

        get_block(pmip->dev, pmip->INODE.i_block[i], buf);
        dp = (DIR *)buf;
        cp = buf;

        while (cp < buf + BLKSIZE) //iterate through the data block
        {
            if (strcmp(dp->name, name) == 0) //found the match
            {

                //first and only //CORRECT
                if (dp->rec_len == BLKSIZE)
                {
                    bdalloc(pmip->dev, pmip->INODE.i_block[i]); //dalloc block
                    pmip->INODE.i_size -= BLKSIZE;              //shrink parent

                    if (pmip->INODE.i_block[i + 1] > 0) //between used blocks
                    {
                        j = i;
                        for (j = i; j < 12; j++)
                        {
                            pmip->INODE.i_block[j] = pmip->INODE.i_block[j + 1];
                            //shift blocks down
                        }
                    }
                    break;
                } //end first and only

                //last entry //CORRECT
                else if (cp + dp->rec_len == buf + BLKSIZE)
                {
                    ndp->rec_len += dp->rec_len;
                    break;
                } //end last entry

                else //first or middle entry //CORRECT
                {    //memcpy is hard
                    tcp = cp + dp->rec_len;
                    tdp = dp;
                    temp_rec_len = dp->rec_len; //hold value for rec_len

                    memcpy(tdp, tcp, (size_t)(BLKSIZE + buf) - (size_t)cp); //shift left

                    tcp = cp;
                    tdp = dp;
                    while (tcp < buf + BLKSIZE - temp_rec_len) //second pointers iterate to last record
                    {
                        if (tcp + tdp->rec_len == buf + BLKSIZE - temp_rec_len) //last entry
                            break;
                        tcp += tdp->rec_len;
                        tdp = (DIR *)tcp;
                    }

                    tdp->rec_len += temp_rec_len; //add deleted rec_len to the last record
                    printf("hi my name is kellan\n");
                    break;
                } // end first or middle entry

            } //end if found match

            ncp = cp;
            ndp = (DIR *)ncp; //ncp and ndp lag behind 1 entry
            cp += dp->rec_len;
            dp = (DIR *)cp;
        } //end iterate through data block
        put_block(pmip->dev, pmip->INODE.i_block[i], buf);
    } //end iterate through blocks
}