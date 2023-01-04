#include "includes/webserv.hpp"
#include <fcntl.h>

void exit_function(char *error)
{
    write (2, error, strlen(error));
    exit (1);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "error : number of arguments\n";
        exit (0);
    }
    Cluster     cluster;

    cluster.put_in_vector(argv[1]);
    if (!cluster.check_point_virgule())
    {
        std::cout << "directives must end by a ;\n";
        return (0);
    }

    cluster.config();

    int                 number_server_max = cluster.get_servers().size();
    int                 nb_fd_ready;
    char                buffer[1024];
    int                 ret;
    struct sigaction	sig;
    struct epoll_event  event_wait[number_server_max][32];
    int                 port[number_server_max];
    int                 number_server = 0;
    socket_class        instance;
    int                 epoll_fd[number_server_max];
    
    /* DECLARATION SOCKET & STRUCTURE SERVEUR */
    SOCKADDR_IN                 struct_server[number_server_max];
    int                         socket_server[number_server_max];
    socklen_t recsize =         sizeof(struct_server);

    /* DECLARATION SOCKET & STRUCTURE CLIENT */
    SOCKADDR_IN                 struct_client[number_server_max];
    int                         socket_client;
    socklen_t crecsize =        sizeof(struct_client);

    
    /*ASSIGNATION DES PORTS*/

    for (int i = 0; i < number_server_max; i++)
    {
        if ((port[i] = cluster.get_servers()[i].get_port()) < 0)
        {
            exit_function((char *)"PORT\n");
        }
    }

    /* CREATION FILE DESCRIPTOR EPOLL */

    for (int i = 0; i < number_server_max; i++)
    {
        if ((epoll_fd[i] = epoll_create(1)) < 0)
        {
            exit_function((char *)"EPOLL CREATE\n");
        }
    }

    /* CREATION DU SOCKET */

    for (int i = 0; i < number_server_max; i++)
    {
        if ((socket_server[i] = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0)
        {
            exit_function((char *)"SOCKET SERVER\n");
        }
    }

    /* ASSIGNATION STRUCTURE FD SERVERS */

    for (int i = 0; i < number_server_max; i++)
    {
        struct_server[i].sin_addr.s_addr =         htonl(INADDR_ANY);  
        struct_server[i].sin_family =              AF_INET;            
        struct_server[i].sin_port =                htons(port[i]);
    }

    for (int i = 0; i < number_server_max; i++)
    {
        int on = 1;
        if (setsockopt(socket_server[i], SOL_SOCKET,  SO_REUSEADDR | SO_REUSEPORT, &on, sizeof(int)) == -1)
        {
               exit_function((char *)"SET SOCKET OPERATION\n");
        }
    }

    /*AFFECTATION D'UN NOM A UNE SOCKET*/

    for (int i = 0; i < number_server_max; i++)
    {
        ret = bind(socket_server[i], (SOCKADDR*)&struct_server[i], recsize);
        if (ret == -1)
        {
            exit_function((char *)"BIND\n");
        }
    }

    /*ECOUTE DES SOCKET SERVEUR*/

    for (int i = 0; i < number_server_max; i++)
    {
        ret = listen(socket_server[i], 32);
        if (ret == -1)
        {
            exit_function((char *)"LISTEN\n");
        }
    }

    /*AJOUT DES FD SERVEUR AU POOL DE FD A MONITORER*/

    for (int i = 0; i < number_server_max; i++)
    {

        struct epoll_event event_server;
        memset(&event_server, 0, sizeof(event_server));
        
        event_server.events  = EPOLLIN;
        event_server.data.fd = socket_server[i];
        ret = epoll_ctl(epoll_fd[i], EPOLL_CTL_ADD, socket_server[i], &event_server);
    }

    /* BOUCLE DE N-SERVEUR NON BLOQUANTE*/

    int i = 0;
    Static_response   rsp_hdl;
    bool    connection_alive = true;
    while (42)
    {
            //std::cout << "Checking for server number " << number_server << " on port " << port[number_server] << std::endl;

            nb_fd_ready = epoll_wait(epoll_fd[number_server], event_wait[number_server], 32, 0 );
            if (nb_fd_ready)
            {
                for (int i = 0; i < nb_fd_ready; i++) 
                {
                    int fd = event_wait[number_server][i].data.fd;

                    if (event_wait[number_server][i].events == EPOLLIN) //SI LE FD TRIGGER EPOLL POUR UNE LECTURE
                    {
                        //std::cout << std::endl << "reception data"  << std::endl;
                        if (fd == socket_server[number_server]) 
                        {
                            //std::cout << "NEW CLIENT ARRIVED" << std::endl; 
                            socket_client = accept(socket_server[number_server], (struct sockaddr*)&struct_client[number_server], &crecsize);

                            int flags = fcntl(socket_client, F_GETFL, 0);
                            fcntl(socket_client, F_SETFL, flags | O_NONBLOCK);

                            struct epoll_event event;
                            memset(&event, 0, sizeof(event));

                            event.events  = EPOLLIN;         
                            event.data.fd = socket_client;
                            epoll_ctl(epoll_fd[number_server], EPOLL_CTL_ADD, socket_client, &event);

                            instance.add_client(socket_client);
                            fd = socket_client;
                        }
                        if (!instance.traitement(fd))
                        {
                            epoll_ctl(epoll_fd[number_server], EPOLL_CTL_DEL, fd, 0);
                            close (fd);
                            //connection_alive = false;
                        }

                        instance.print_client(fd);

                        if (!instance.check_suspended(fd))
                        {
                            //Dimitri
                            std::map<int, client_buffer*> it = instance.get_tree();

                            Request_parser  req_pars(it.find(fd)->second, fd); // fd
                            //std::cout << "rsp_hdl.getAllowedMethods()0: " << rsp_hdl.getAllowedMethods() << std::endl;
                            req_pars.parse_request();
                            
                            rsp_hdl.setRequestParser(req_pars);
                            
                            Request request(req_pars.getRequestMethod(), req_pars.getRequestHostname(), req_pars.getRequestUri(), "1.2.3.4", 13878);

                            AlgoServer algo1(cluster, request);
                            Server     server_choosen;
                            server_choosen = algo1.run();
                            //server_choosen.print_server();

                            AlgoLocation algo2(server_choosen, request);
                            Location location_chosen = algo2.run();
                            

                            handle_request_execution(req_pars, location_chosen, rsp_hdl, fd);
                            //std::cout << "ICI: " << rsp_hdl.getAllowedMethods() << std::endl;
                            location_chosen.print_location();
                            struct epoll_event event;
                            memset(&event, 0, sizeof(event));
                            
                            event.events  = EPOLLOUT;         
                            event.data.fd = fd;
                            epoll_ctl(epoll_fd[number_server], EPOLL_CTL_MOD, fd, &event);
                            
                            
                        }
                    }

                    if (event_wait[number_server][i].events == EPOLLOUT) //SI LE FD TRIGGER EPOLL POUR UNE ECRITURE
                    {
                        //instance.send_data ( fd , number_server );
                        std::cout << "DEBUT" << std::endl;
                        std::cout.write(rsp_hdl.getBuffer(), rsp_hdl.getLength()) << "FIN" << std::endl;
                        //std::cout << "rsp_hdl.getAllowedMethods()667: " << rsp_hdl.getAllowedMethods() << std::endl;
                        //if (rsp_hdl.getAllowedMethods())
                        send(fd, rsp_hdl.getBuffer(), rsp_hdl.getLength(), 0);
                        struct epoll_event event;
                        memset(&event, 0, sizeof(event));

                        event.events  = EPOLLIN;
                        event.data.fd = fd;
                        epoll_ctl(epoll_fd[number_server], EPOLL_CTL_MOD, fd, &event);

                        instance.refresh_client(fd);

                        //std::cout << "eppolin" << std::endl;
                    }
                }
            }
            /*else
                std::cout << "no triggered" << std::endl;*/

            if ( (i = instance.check_time ()) > 0)
            {
                epoll_ctl(epoll_fd[number_server], EPOLL_CTL_DEL, i, 0);
                close (i);
            }

            //std::cout << std::endl << "->"  << std::endl;
            number_server++;
            if (number_server == number_server_max)
                number_server = 0;
    }

    return (0);
}