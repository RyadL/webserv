#include "webserv.hpp"
#include <fcntl.h>

void	handler(int signal)
{
	if (signal == SIGINT)
        throw std::runtime_error ("signal == SIGINT");
	if (signal == SIGQUIT)
		throw std::runtime_error ("signal == SIGINT");
}


struct Epoll_struct
{
    Epoll_struct(char **argv)
    {
        cluster.put_in_vector(argv[1]);
        if (!cluster.check_point_virgule())
            throw std::invalid_argument ("directives must end by a ;");
        cluster.config();
        number_server_max = cluster.get_servers().size();
        ret = 0;
        event_wait = new struct epoll_event* [number_server_max];
        for (int i = 0 ; i < number_server_max; i++)
            event_wait[i] = new struct epoll_event [32];
        port = new int [number_server_max];
        epoll_fd = new int [number_server_max];
        number_server = 0;
        fd = 0;
        memset(&sig, 0, sizeof(struct sigaction));
        sig.sa_handler = &handler;
	    sigaction(SIGINT, &sig, 0);
	    sigaction(SIGQUIT, &sig, 0);
    }

    ~Epoll_struct()
    {
        for (int i = 0 ; i < number_server_max; i++)
            delete [] event_wait[i];
        delete [] event_wait;
        delete [] port;
        delete [] epoll_fd;
    }

    Cluster                     cluster;
    int                         number_server_max;
    int                         nb_fd_ready;
    int                         ret;
    struct epoll_event          **event_wait;
    int                         *port;
    int                         number_server;
    int                         *epoll_fd;
    Static_response             rsp_hdl;
    int                         fd;
    struct sigaction	        sig;
};

class Epoll_class 
{
    public :

    void launch_server (Epoll_struct *epoll_struct, socket_class *instance)
    {
        /* DECLARATION SOCKET & STRUCTURE SERVEUR */
        SOCKADDR_IN                 struct_server[epoll_struct->number_server_max];
        int                         socket_server[epoll_struct->number_server_max];
        socklen_t recsize =         sizeof(struct_server);

        /* DECLARATION SOCKET & STRUCTURE CLIENT */
        SOCKADDR_IN                 struct_client[epoll_struct->number_server_max];
        int                         socket_client;
        socklen_t crecsize =        sizeof(struct_client);


        /*ASSIGNATION DES PORTS*/
        for (int i = 0; i < epoll_struct->number_server_max; i++)
        {
            if ((epoll_struct->port[i] = epoll_struct->cluster.get_servers()[i].get_port()) < 0)
                throw std::runtime_error ("ports");
        }

        /* CREATION FILE DESCRIPTOR EPOLL */
        for (int i = 0; i < epoll_struct->number_server_max; i++)
        {
            if ((epoll_struct->epoll_fd[i] = epoll_create(1)) < 0)
                throw std::runtime_error ("epoll_create");
        }

        /* CREATION DU SOCKET */
        for (int i = 0; i < epoll_struct->number_server_max; i++)
        {
            if ((socket_server[i] = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0)
                throw std::runtime_error ("socket_server");
        }

        /* ASSIGNATION STRUCTURE FD SERVERS */

        for (int i = 0; i < epoll_struct->number_server_max; i++)
        {
            struct_server[i].sin_addr.s_addr =         htonl(INADDR_ANY);  
            struct_server[i].sin_family =              AF_INET;            
            struct_server[i].sin_port =                htons(epoll_struct->port[i]);
        }

        for (int i = 0; i < epoll_struct->number_server_max; i++)
        {
            int on = 1;
            if (setsockopt(socket_server[i], SOL_SOCKET,  SO_REUSEADDR | SO_REUSEPORT, &on, sizeof(int)) == -1)
                throw std::runtime_error ("set socket operation");
        }

        /*AFFECTATION D'UN NOM A UNE SOCKET*/
        for (int i = 0; i < epoll_struct->number_server_max; i++)
        {
            epoll_struct->ret = bind(socket_server[i], (SOCKADDR*)&struct_server[i], recsize);
            if (epoll_struct->ret == -1)
                throw std::runtime_error ("epoll_create");
        }

        /*ECOUTE DES SOCKET SERVEUR*/
        for (int i = 0; i < epoll_struct->number_server_max; i++)
        {
            epoll_struct->ret = listen(socket_server[i], 32);
            if (epoll_struct->ret == -1)
                throw std::runtime_error ("listen");
        }

        /*AJOUT DES FD SERVEUR AU POOL DE FD A MONITORER*/

        for (int i = 0; i < epoll_struct->number_server_max; i++)
        {
            struct epoll_event event_server;
            memset(&event_server, 0, sizeof(event_server));

            event_server.events  = EPOLLIN;
            event_server.data.fd = socket_server[i];
            epoll_struct->ret = epoll_ctl(epoll_struct->epoll_fd[i], EPOLL_CTL_ADD, socket_server[i], &event_server);
        }

        /* BOUCLE DE N-SERVEUR NON BLOQUANTE*/

        while (42)
        {
                //std::cout << "Checking for server number " << number_server << " on port " << port[number_server] << std::endl;

                epoll_struct->nb_fd_ready = epoll_wait(epoll_struct->epoll_fd[epoll_struct->number_server], epoll_struct->event_wait[epoll_struct->number_server], 32, 0 );
                if (epoll_struct->nb_fd_ready)
                {
                    for (int i = 0; i < epoll_struct->nb_fd_ready; i++) 
                    {
                        int fd = epoll_struct->event_wait[epoll_struct->number_server][i].data.fd;

                        if (epoll_struct->event_wait[epoll_struct->number_server][i].events == EPOLLIN) //SI LE FD TRIGGER EPOLL POUR UNE LECTURE
                        {
                            //std::cout << std::endl << "reception data"  << std::endl;
                            if (fd == socket_server[epoll_struct->number_server]) 
                            {
                                //std::cout << "NEW CLIENT ARRIVED" << std::endl; 
                                socket_client = accept(socket_server[epoll_struct->number_server], (struct sockaddr*)&struct_client[epoll_struct->number_server], &crecsize);

                                int flags = fcntl(socket_client, F_GETFL, 0);
                                fcntl(socket_client, F_SETFL, flags | O_NONBLOCK);

                                struct epoll_event event;
                                memset(&event, 0, sizeof(event));

                                event.events  = EPOLLIN;         
                                event.data.fd = socket_client;
                                epoll_ctl(epoll_struct->epoll_fd[epoll_struct->number_server], EPOLL_CTL_ADD, socket_client, &event);

                                instance->add_client(socket_client);
                                epoll_struct->rsp_hdl.add_client(socket_client);
                                fd = socket_client;
                            }
                            if (!instance->traitement(fd))
                            {
                                epoll_ctl(epoll_struct->epoll_fd[epoll_struct->number_server], EPOLL_CTL_DEL, fd, 0);
                                close (fd);
                            }

                            if (!instance->check_suspended(fd))
                            {
                                instance->print_client(fd);

                                std::map<int, client_buffer*> it = instance->get_tree();

                                Request_parser  req_pars(it.find(fd)->second, fd);
                                req_pars.parse_request();

                                epoll_struct->rsp_hdl.setRequestParser(req_pars);

                                Request request(req_pars.getRequestMethod(), req_pars.getRequestHostname(), req_pars.getRequestUri(), "1.2.3.4", 13878);

                                AlgoServer algo1(epoll_struct->cluster, request);

                                Server     server_choosen;

                                server_choosen = algo1.run();

                                //server_choosen.print_server();

                                AlgoLocation algo2(server_choosen, request);

                                Location location_chosen = algo2.run();

                                handle_request_execution(req_pars, location_chosen, epoll_struct->rsp_hdl, fd);
                                //location_chosen.print_location();
                                struct epoll_event event;
                                memset(&event, 0, sizeof(event));

                                event.events = EPOLLOUT;         
                                event.data.fd = fd;
                                epoll_ctl(epoll_struct->epoll_fd[epoll_struct->number_server], EPOLL_CTL_MOD, fd, &event);


                            }
                        }

                        if (epoll_struct->event_wait[epoll_struct->number_server][i].events == EPOLLOUT) //SI LE FD TRIGGER EPOLL POUR UNE ECRITURE
                        {
                            epoll_struct->rsp_hdl.send_response();
    
                            struct epoll_event event;
                            memset(&event, 0, sizeof(event));

                            event.events  = EPOLLIN;
                            event.data.fd = fd;
                            epoll_ctl(epoll_struct->epoll_fd[epoll_struct->number_server], EPOLL_CTL_MOD, fd, &event);

                            instance->refresh_client(fd);


                        }
                    }
                }
                if (int fd = instance->check_time () > 0)
                {
                    epoll_ctl(epoll_struct->epoll_fd[epoll_struct->number_server], EPOLL_CTL_DEL, fd, 0);
                    close (fd);
                }
                epoll_struct->number_server++;
                if (epoll_struct->number_server == epoll_struct->number_server_max)
                    epoll_struct->number_server = 0;
        }
    }
};  