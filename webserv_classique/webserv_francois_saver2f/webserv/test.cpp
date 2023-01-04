#include <cstddef>
#include <sys/socket.h>
 #include <limits.h> /* for OPEN_MAX */
#include <sys/epoll.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include <arpa/inet.h>
#include <strings.h>
#include <cstring>
#include <iostream>
#include <unistd.h>

# define MAX_EVENTS 10

int main(int argc, char **argv)
{

    struct epoll_event  ev, events[MAX_EVENTS];
    struct sockaddr_in  servaddr;
    int fd_listen;
    int epoll_fd;
    char buffer[663];

    epoll_fd = epoll_create(1);

    fd_listen = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(sockaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(13667);
    int optval = 1;
    setsockopt(fd_listen, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
    if (bind(fd_listen, (sockaddr*)&servaddr, sizeof(sockaddr)) == -1)
        return 1;

    listen(fd_listen, 10);

    struct epoll_event event_server;
    event_server.events  = EPOLLIN | EPOLLOUT;
    event_server.data.fd = fd_listen;

    std::cout << "EPOLL_FD: " << epoll_fd << std::endl;

    std::cout << "FD_LISTEN: " << fd_listen << std::endl;

    std::cout << "EVENT_SERVER: " << event_server.data.fd << std::endl;
    //bzero(&event_server, sizeof(epoll_event));
    int ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd_listen, &event_server);
    if (ret == -1)
        return 1;

    int connect_fd;
    struct sockaddr_in cliaddr;
    socklen_t clilen = sizeof(cliaddr);
    int ready;
    int n;
    struct epoll_event event;
    bzero(&event.events, sizeof(event.events));
    while (1)
    {
        std::cout << "OKAY0" << std::endl;
        ready = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        std::cout << "READY: " << ready << std::endl;
        for (int    i = 0; i < ready; i++)
            std::cout << "events[i].data.fd: " << events[i].data.fd << std::endl;
        
        //if (events[0].data.fd == 5)
        //    return 0;
        if (ready)
        {

        
            for (int    i = 0; i < ready; i++)
            {
                int fd = events[i].data.fd;
                std::cout << "events[i].data.fd: " << events[i].data.fd << std::endl;
                //std::cout << "connect_fd: " << connect_fd << " et fd_listen: " << fd_listen << std::endl;
                if ((events[i].events & EPOLLIN) == EPOLLIN)
                {
                    std::cout << "OKAY1" << std::endl;
                    if (events[i].data.fd == fd_listen)
                    {
                        std::cout << "OKAY2" << std::endl;
                        connect_fd = accept(fd_listen, (sockaddr*)&cliaddr, &clilen);
                        std::cout << "OKAY22" << std::endl;
                        //struct epoll_event event;
                        //bzero(&event.events, sizeof(event.events));
                        event.events = EPOLLIN;
                        event.data.fd = connect_fd;
                        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connect_fd, &event);
                        
                    }
                    
                    else
                    {
                        std::cout << "OKAY3" << std::endl;
                        //return 0;
                        n = recv(connect_fd, buffer, sizeof(buffer), 0);
                        if (n == 0)
                        {
                            std::cout << "Client closed connection" << std::endl;
                            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, connect_fd, &event);
                            close(connect_fd);
                        }
                        std::cout << "N: " << n << std::endl;
                        buffer[n] = 0;
                        std::cout << "BUFFER: " << buffer << std::endl;

                        event.events = EPOLLOUT;
                        event.data.fd = connect_fd;
                        epoll_ctl(epoll_fd, EPOLL_CTL_MOD, connect_fd, &event); 
                    }
                    

                        //return 0;
                    
                        
                    
                }
                if (events[i].events == EPOLLOUT)
                {
                    std::cout << "OKAY4" << std::endl;
                    int test = send(events[i].data.fd, buffer, std::strlen(buffer), 0);
                    std::cout << "test: " << test << std::endl;
                    struct epoll_event event;
                    bzero(&event.events, sizeof(event.events));
                    event.events = EPOLLIN;
                    event.data.fd = connect_fd;
                    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, connect_fd, &event);
                    //return 0;         
                }
                
            }
        }
        
    }

    return 0;
}