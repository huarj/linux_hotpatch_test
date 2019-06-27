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


#define DEBUG(fmt,...)	do {printf(fmt, __VA_ARGS__);} while (0)
#define INFO(fmt,...) do {printf(fmt, __VA_ARGS__);} while (0)
#define NOTICE(fmt,...) do {printf(fmt, __VA_ARGS__);} while (0)
#define ERROR(fmt,...) do {printf(fmt, __VA_ARGS__);} while (0)


int set_data(pid_t pid, int addr, void *val, int vlen)
{
    int  i;
    int  addr0 = addr & ~3;
    int  len = (((addr + vlen) - addr0) + 3)/4;
    int  *lv = malloc(len * sizeof(int));

    DEBUG("peek: %d, addr0 = %x, addr-addr0 = %d", len, addr0, addr-addr0);
    for (i = 0; i < len; i++) {
	if (i % 4 == 0) {
	    DEBUG("\n %p  ", (void *)(addr0 + i * sizeof(int)));
	}
	lv[i] = ptrace(PTRACE_PEEKDATA, pid, 
		       addr0 + i * sizeof(int), NULL);
	if (lv[i] == -1 && errno != 0) {
	    perror("ptrace peek");
	    return -1;
	}
	DEBUG("%08x ", lv[i]);
    }
    memcpy((char *)lv + (addr - addr0), val, vlen);
    DEBUG("\npoke: %d", len);
    for (i = 0; i < len; i++) {
	if (i % 4 == 0) {
	    DEBUG("\n %p  ", (void *)(addr0 + i * sizeof(int)));
	}
	if (ptrace(PTRACE_POKEDATA, pid,
		   addr0 + i * sizeof(int), lv[i]) < 0) {
	    perror("ptrace poke");
	    return -1;
	}
	DEBUG("%08x ", lv[i]);
    }
    DEBUG("%s", "\n"); /* XXX */
    return 0;
}


//./jmp "31282" "4195712" "4195729" "0"
int main(int argc, char *argv[])
{
    unsigned int addr;
    unsigned int addr2;
    int jmp_relative;
    char jmpbuf[5];
    pid_t target_pid;
    int status = -1; 

    if (argc == 1)
    {
        printf("usage error\n");
        return 0;
    }

    target_pid = atoi(argv[1]);

    
    addr = atoi(argv[2]); //source
    addr2 = atoi(argv[3]);//target
    
    jmp_relative = addr2 - (addr + 5);
    
    printf("addr2=%p addr=%p \n", addr2, addr);
    printf("jmp relative %ld (0x%08lx)\n", jmp_relative, jmp_relative);
    
    jmpbuf[0] = 0xe9; /* jmp */
    memcpy(jmpbuf+1, &jmp_relative, sizeof(int));
    
    status = 1;


    printf("jmpbuf = 0x%02x%02x%02x%02x%02x\n", jmpbuf[0], jmpbuf[1], jmpbuf[2], jmpbuf[3], jmpbuf[4]);
    
    if (ptrace(PTRACE_ATTACH, target_pid, NULL, NULL) < 0) 
    {
    	perror("ptrace attach");
    	exit(-2);
    }
    
    DEBUG("attached %d\n", target_pid);
    wait(NULL);
    
    if (set_data(target_pid, addr, jmpbuf, sizeof(jmpbuf)) < 0) 
    {
        DEBUG("E: jmp %p %p failed.\n", (void *)addr, (void *)addr2);
        
        ptrace(PTRACE_DETACH, target_pid, NULL, NULL);
        DEBUG("detached %d\n", target_pid);
        
        exit(-3);        
    }

    ptrace(PTRACE_DETACH, target_pid, NULL, NULL);
    DEBUG("detached %d\n", target_pid);
    exit(status);
}


