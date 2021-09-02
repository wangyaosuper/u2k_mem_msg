#include <stdio.h>
#include <unistd.h>
#include <fcntl.h> 
#include <stdarg.h>



int forkScript(char * buffer, int size, char * scriptPath, ...){
    va_list ap;
    int df[2];
    pid_t pid;
    int res;
    int si;
    char * arg;
    char * argArray[1024];
    int pos = 0;
    if (pipe(df)) {
        printf("ERROR: pipe failed\n");
        return -1;
    }
    
    pid = fork();

    if (0 == pid){   // child process
        close(df[0]);
        dup2(df[1], 1);
        dup2(df[1], 2);
        va_start(ap, scriptPath);
        pos = 0;
        arg = "a";
        while (NULL != arg){
            arg = va_arg(ap , char*);
            argArray[pos] = arg;
            ++pos;
        }
        va_end(ap);
        res = execvp(scriptPath, argArray);
        if (res != 0){
            printf("failed to wrong the execlp function\n");
        }
        close(df[1]);
        printf("the shell run finised.");
        exit(0);
    }
    else if(pid > 0){   // parent process
        wait(pid);
        close(df[1]);
        si = 1;
        while(si > 0){
            si = read(df[0], buffer, size - 1);
            buffer[si] = '\0';
            printf(buffer);
        }
        return 0;
    }
    else if(pid < 0){
        printf("ERROR: fork failed.\n");
        return -1;
    }

    return 0;
}
