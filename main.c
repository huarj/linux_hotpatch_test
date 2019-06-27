#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <sys/mman.h>


void myprint()
{
    printf("this is old func\n");
    return;
}


void new_myprint()
{
    printf("this is new func\n");
    return;
}

int main(int argc, char *argv[])
{
    while(1)
    {
        myprint();
        sleep(1);
    }


    return 0;
}


