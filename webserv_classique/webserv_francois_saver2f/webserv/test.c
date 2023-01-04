//#include <string>
//#include <iostream>
//#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


int main(int argc, char *argv[], char *env[])
{
    int pid1;
    int pid2;
    int status;
    int fd_pipe[2];

    if (pipe(fd_pipe) == -1)
        return 1;
    pid1 = fork();
    if (!pid1)
    {
        close(fd_pipe[0]);
        dup2(fd_pipe[1], STDOUT_FILENO);
        close(fd_pipe[1]);
        //write(STDOUT_FILENO, "request_examples.txt", 21);
        write(STDOUT_FILENO, "rtest", 6);
        //char *const args[] = {"/usr/bin/ls", "-l", NULL};
        //execve(args[0], args, env);
        exit(0);
    }
    waitpid(pid1, &status, 0);
    pid2 = fork();
    if (!pid2)
    {
        close(fd_pipe[1]);
        dup2(fd_pipe[0], STDIN_FILENO);
        close(fd_pipe[0]);
        
        //std::string test("THIS A phrase de test!");
        char *const args[] = {"/usr/bin/cat", NULL};
        execve(args[0], args, env);
        exit(1);
    }
    close(fd_pipe[0]);
    close(fd_pipe[1]);
    waitpid(pid2, &status, 0);
    
    return 0;
}


