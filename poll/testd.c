#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

int main()
{
    pid_t pid = fork();
    if( pid != 0)
    {
        exit(0);
    }

    setsid();
    pid = fork();
    if( pid != 0)
    {
        exit(0);
    }

    chdir("/");

    umask(0);

    int maxfd = getdtablesize();
    int i = 0;
    for( ; i < maxfd; i++)
    {
        close(i);
    }

    while( 1 )
    {
        FILE *fp = fopen("tmp/xingxing.log", "a");
        if(fp == NULL)
        {
            break;
        }

        time_t t;
        time(&t);

        fprintf(fp, "CURRENT TIME:%s",asctime(localtime(&t)) );
        fclose(fp);

        sleep(5);
    }
    exit(0);
}
