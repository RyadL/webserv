//#include <string>
//#include <iostream>
//#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>

/*
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
        std::cout << "BON LA C VRéMENT 1 TAIST" << std::endl;
        //write(STDOUT_FILENO, "BON LA C VRéMENT 1 TAIST", 20);
        exit(0);
    }
    waitpid(pid1, &status, 0);
    pid2 = fork();
    if (!pid2)
    {
        close(fd_pipe[1]);
        dup2(fd_pipe[0], STDIN_FILENO);
        close(fd_pipe[0]);
        char *const args[] = {"/usr/bin/tee", NULL};
        execve(args[0], args, env);
        exit(1);
    }
    close(fd_pipe[0]);
    close(fd_pipe[1]);
    waitpid(pid2, &status, 0);
    
    return 0;
}
*/
/*
int main(int argc, char *argv[], char *env[])
{
    int pid1;
    int pid2;
    int status;
    int fd_pipe[2];



    
    //close(fd_pipe[1]);
    //write(STDOUT_FILENO, "BON LA C VRéMENT 1 TAIST", 26);
    
        
    
    pid2 = fork();
    if (!pid2)
    {
        if (pipe(fd_pipe) == -1)
            return 1;
        int save_out = dup(1);
        dup2(fd_pipe[1], STDOUT_FILENO);
        close(fd_pipe[1]);
        //std::cout << "BON LA C VRéMENT 1 TAIST667";
        write(STDOUT_FILENO, "BON LA C VRéMENT 1 TAIST", 20);
        dup2(fd_pipe[0], STDIN_FILENO);
        close(fd_pipe[0]);
        char *const args[] = {"/usr/bin/cat", NULL};
        dup2(STDERR_FILENO, STDOUT_FILENO);
        close(save_out);
        execve(args[0], args, env);
        std::cout << "OKAY0" << std::endl;
        exit(1);
    }
    //close(fd_pipe[0]);
    //close(fd_pipe[1]);
    std::cout << "OKAY1" << std::endl;
    waitpid(pid2, &status, 0);
    std::cout << "OKAY2" << std::endl;
    
    return 0;
}
*/
/*
int main(int argc, char *argv[], char *env[])
{
    int pid1;
    int pid2;
    int status;
    int fd_pipe[2];



    
    //close(fd_pipe[1]);
    //write(STDOUT_FILENO, "BON LA C VRéMENT 1 TAIST", 26);
    
        
    
    pid2 = fork();
    if (!pid2)
    {
        if (pipe(fd_pipe) == -1)
            return 1;
        dup2(fd_pipe[0], STDIN_FILENO);
        close(fd_pipe[0]);
        int save_out = dup(1);
        dup2(fd_pipe[1], STDOUT_FILENO);
        close(fd_pipe[1]);
        std::cout << "BON LA C VRéMENT 1 TAIST667";
        std::cout << "PUTAIN CA MARCHE";// << std::endl;
        std::cout.flush();
        //write(STDOUT_FILENO, "BON LA C VRéMENT 1 TAIST667", 29);
        //std::cout.write("BON LA C VRéMENT 1 TAIST667", 29);
        //write(STDOUT_FILENO, "BON LA C VRéMENT 1 TAIST\n", 27);
        //dup2(fd_pipe[0], STDIN_FILENO);
        //close(fd_pipe[0]);
        char *const args[] = {"/usr/bin/tee", NULL};
        dup2(save_out, STDOUT_FILENO);
        close(save_out);
        execve(args[0], args, env);
        std::cout << "OKAY0" << std::endl;
        exit(1);
    }
    //close(fd_pipe[0]);
    //close(fd_pipe[1]);
    //std::cout << "OKAY1" << std::endl;
    waitpid(pid2, &status, 0);
    //std::cout << "OKAY2" << std::endl;
    
    return 0;
}
*/

// PAS OUBLIER DE FLUSH COUT

int main(int argc, char *argv[], char *env[])
{
    int pid1;
    int pid2;
    int status;
    



    
    //close(fd_pipe[1]);
    //write(STDOUT_FILENO, "BON LA C VRéMENT 1 TAIST", 26);
    
        
    
    pid2 = fork();
    if (!pid2)
    {
        int fd_pipe[2];
        if (pipe(fd_pipe) == -1)
            return 1;
        dup2(fd_pipe[0], STDIN_FILENO);
        close(fd_pipe[0]);
        int save_out = dup(1);
        dup2(fd_pipe[1], STDOUT_FILENO);
        close(fd_pipe[1]);
        std::cout << "fname=John";
        //std::cout << "PUTAIN CA MARCHE";// << std::endl;
        std::cout.flush();
        //write(STDOUT_FILENO, "BON LA C VRéMENT 1 TAIST667", 29);
        //std::cout.write("BON LA C VRéMENT 1 TAIST667", 29);
        //write(STDOUT_FILENO, "BON LA C VRéMENT 1 TAIST\n", 27);
        //dup2(fd_pipe[0], STDIN_FILENO);
        //close(fd_pipe[0]);
        //{"/home/dimitri/Documents/42/webserv/post.php"
        char *const args[] = {"/usr/bin/tee", NULL};
        dup2(save_out, STDOUT_FILENO);
        close(save_out);
        execve(args[0], args, env);
        std::cout << "OKAY0" << std::endl;
        exit(1);
    }
    //close(fd_pipe[0]);
    //close(fd_pipe[1]);
    //std::cout << "OKAY1" << std::endl;
    waitpid(pid2, &status, 0);
    //std::cout << "OKAY2" << std::endl;
    
    return 0;
}