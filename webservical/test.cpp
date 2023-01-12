#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <iostream>
#include <unistd.h>

int main()
{
    int fd_pipe[2];

    pipe(fd_pipe);
    std::cout << fcntl(fd_pipe[1], F_GETPIPE_SZ) << std::endl;
    return 0;
}