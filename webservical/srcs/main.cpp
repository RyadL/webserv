#include <fcntl.h>
#include "../includes/Epoll_class.hpp"
#include <signal.h>


int main(int argc, char **argv)
{
    socket_class        instance;
    struct sigaction	sig;

    try {
        Epoll_struct        epoll_struct(argv);
        Epoll_class         epoll_class; 

        epoll_class.launch_server(&epoll_struct, &instance);
    }

    catch( const std::exception & e ) {
        std::cerr << e.what() << std::endl;
        instance.free_tree();
    }
}