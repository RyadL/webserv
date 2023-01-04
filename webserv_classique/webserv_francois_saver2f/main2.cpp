

//#include "Cluster.hpp"
//#include "socket_class.hpp"
//#include "client_buffer.hpp"
//#include <fcntl.h>
#include "includes/webserv.hpp"
/* DECLARATION SOCKET & STRUCTURE SERVEUR */
SOCKADDR_IN                 struct_server[3];
int                         socket_server[3];
socklen_t recsize =         sizeof(struct_server);

/* DECLARATION SOCKET & STRUCTURE CLIENT */
SOCKADDR_IN                 struct_client[3];
int                         socket_client;
socklen_t crecsize =        sizeof(struct_client);

int epoll_fd[3];

void exit_function(char *error)
{
    write (2, error, strlen(error));
    exit (1);
}

void	handler(int signal)
{
	if (signal == SIGINT)
	{
        close (socket_server[0]);
        close (socket_server[1]);
        close (socket_server[2]);
        close (epoll_fd[0]);
        close (epoll_fd[1]);
        close (epoll_fd[2]);
		exit_function((char *)"SIGINT\n");
	}
	if (signal == SIGQUIT)
    {
        close (socket_server[0]);
        close (socket_server[1]);
        close (socket_server[2]);
        close (epoll_fd[0]);
        close (epoll_fd[1]);
        close (epoll_fd[2]);
		exit_function((char *)"SIGQUIT\n");
	}
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

    //std::cout << "Number max of server = " << number_server_max << std::endl;

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

    /* GESTION DES SIGNAUX */

    memset(&sig, 0, sizeof(struct sigaction));
    sig.sa_handler = &handler;
	sigaction(SIGINT, &sig, 0);
	sigaction(SIGQUIT, &sig, 0);

    /* CREATION DU SOCKET */

    for (int i = 0; i < number_server_max; i++)
    {
        if ((socket_server[i] = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0)
        {
            exit_function((char *)"SOCKET SERVER\n");
        }
    }
#include <fcntl.h>
    //std::cout << std::endl << "Les sockets server sont maintenant ouverte en mode TCP/IP" << std::endl;

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
        ret = listen(socket_server[i], 15);
        if (ret == -1)
        {
            exit_function((char *)"LISTEN\n");
        }
    }

    /*AJOUT DES FD SERVEUR AU POOL DE FD A MONITORER*/

    for (int i = 0; i < number_server_max; i++)
    {
        struct epoll_event event_server;
        event_server.events  = EPOLLIN;
        event_server.data.fd = socket_server[i];
        ret = epoll_ctl(epoll_fd[i], EPOLL_CTL_ADD, socket_server[i], &event_server);
    }

    /* BOUCLE DE N-SERVEUR NON BLOQUANTE*/

    int i = 0;
    Static_response   rsp_hdl;
    struct epoll_event event{0};
    //std::memset((void *)event, 0, sizeof(event));
    while (42)
    {
            //std::cout << "Checking for server number " << number_server << " on port " << port[number_server] << std::endl;
            //std::cout << "OKAY0" << std::endl;
            nb_fd_ready = epoll_wait(epoll_fd[number_server], event_wait[number_server], 32, 0 );
            if (nb_fd_ready)
            {
                std::cout << "NB_FD_READY: " << nb_fd_ready << std::endl;
                for (int i = 0; i < nb_fd_ready; i++) 
                {
                    int fd = event_wait[number_server][i].data.fd;

                    if (event_wait[number_server][i].events == EPOLLIN) //SI LE FD TRIGGER EPOLL POUR UNE LECTURE
                    {
                        std::cout << std::endl << "reception data"  << std::endl;
                        if (fd == socket_server[number_server]) 
                        {
                            socket_client = accept(socket_server[number_server], (struct sockaddr*)&struct_client[number_server], &crecsize);
                            std::cout << "NOUVEAU CLIENT" << std::endl;
                            int flags = fcntl(socket_client, F_GETFL, 0);
                            fcntl(socket_client, F_SETFL, flags | O_NONBLOCK);

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
                        }
                        else if (!instance.print_client(fd))
                        {
                                               
                            std::map<int, client_buffer*> client_tree = instance.get_tree();

                            
                            
                            Request_parser  req_pars(client_tree.find(fd)->second->short_header, fd); // fd
                                  
                            req_pars.parse_request();

                            rsp_hdl.setRequestParser(req_pars);

                            
                               
                            Request request(req_pars.getRequestMethod(), req_pars.getRequestHostname(), req_pars.getRequestUri(), "1.2.3.4", 13878);

                            AlgoServer algo1(cluster, request);
                            Server     server_choosen;
                            server_choosen = algo1.run();
                            //server_choosen.print_server();

                            AlgoLocation algo2(server_choosen, request);
                            Location location_chosen = algo2.run();    
                            location_chosen.print_location();
                            event.events  = EPOLLOUT;         
                            event.data.fd = fd;
                            epoll_ctl(epoll_fd[number_server], EPOLL_CTL_MOD, fd, &event);
                            std::cout << "ON EST LA0" << std::endl;
                            handle_request_execution(req_pars, location_chosen, rsp_hdl, fd);
                            
                        }
                        else
                        {
                            //std::cout << "ON EST LA667974" << std::endl;
                            //instance.refresh_client( instance.get_tree().find(fd));
                            //exit(0);
                        }
                    }

                    if (event_wait[number_server][i].events == EPOLLOUT) //SI LE FD TRIGGER EPOLL POUR UNE ECRITURE
                    {
                        //instance.send_data ( fd , number_server );
                        std::cout << "rsp_hdl.getBuffer()\n" << rsp_hdl.getBuffer() << std::endl;
                        //instance.send_response(fd, rsp_hdl.getBuffer(), rsp_hdl.getLength());
                        //instance
                        event.events  = EPOLLIN;
                        event.data.fd = fd;
                        epoll_ctl(epoll_fd[number_server], EPOLL_CTL_MOD, fd, &event);

                        //std::cout << "eppolin" << std::endl;
                    }
                }
            }
            /*else
                std::cout << "no triggered" << std::endl;*/

            /*
            if ( (i = instance.check_time ()) > 0)
            {
                epoll_ctl(epoll_fd[number_server], EPOLL_CTL_DEL, i, 0);
                //close (i);
            }
            */

            //std::cout << std::endl << "->"  << std::endl;
            number_server++;
            if (number_server == number_server_max)
                number_server = 0;
    }

    return (0);
}

// CHOIX A FAIRE:
// SI REQUETE NON SUPPORTER PAR LE SERVER --> RENVOYER RIEN COMME LE TESTER OU 405 COMME NGINX/RFC?