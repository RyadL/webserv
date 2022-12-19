
/*
#include "Cluster.hpp"
#include "Request.hpp"
#include "AlgoLocation.hpp"
#include "AlgoServer.hpp"
#include "webserv/request_parser.hpp"
#include "webserv/cgi_handler.hpp"
#include "response/static_response.hpp"
#include "Location.hpp"
*/
#include "includes/webserv.hpp"

/* DECLARATION SOCKET & STRUCTURE SERVEUR */
SOCKADDR_IN                 struct_server[3];
SOCKET                      socket_server[3];
socklen_t recsize =         sizeof(struct_server);

/* DECLARATION SOCKET & STRUCTURE CLIENT */
SOCKADDR_IN                 struct_client[3];
SOCKET                      socket_client;
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
    //cluster.print_cluster();

    int                 number_server_max = cluster.get_servers().size();
    int                 nb_fd_ready;
    char                buffer[39203];
    int                 ret;
    struct sigaction	sig;
    struct epoll_event  event_wait[number_server_max][32];
    int                 port[number_server_max];
    int                 number_server = 0;

    std::cout << "Number max of server = " << number_server_max << std::endl;
    //std::cout << "SIZE: " << sizeof(buffer) << std::endl;
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

    std::cout << std::endl << "Les sockets server sont maintenant ouverte en mode TCP/IP\n" << std::endl;

    /* ASSIGNATION STRUCTURE FD SERVERS */

    for (int i = 0; i < number_server_max; i++)
    {
        struct_server[i].sin_addr.s_addr =         htonl(INADDR_ANY);  
        struct_server[i].sin_family =              AF_INET;            
        struct_server[i].sin_port =                htons(port[i]);
    }

    /*AFFECTATION D'UN NOM A UNE SOCKET*/

    for (int i = 0; i < number_server_max; i++)
    {
        int optval = 1;
        setsockopt(socket_server[i], SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
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
        event_server.events  = EPOLLIN | EPOLLOUT;
        event_server.data.fd = socket_server[i];
        ret = epoll_ctl(epoll_fd[i], EPOLL_CTL_ADD, socket_server[i], &event_server);
    }
    /*

                    
    int fd = 4;

    Request_parser  req_pars(buffer, fd); // fd
                        
    req_pars.parse_request();

    std::cout << "METHOD: " << req_pars.getRequestMethod() << std::endl;
    std::cout << "HOSTNAME: " << req_pars.getRequestHostname() << std::endl;
    std::cout << "URI: " << req_pars.getRequestUri() << std::endl;
    Request request("GET", "www.example.com", "/directory/hello.php", "1.2.3.4", 13878);


    //une fois qu'on a rempli la classe Request avec l'ip, le port, la methode, le domaine et le path(ce qui suit le nom de domaine dans l'url)
    // on lance l'algorithme qui permet de definir a quelle block server la requete correspond
    //il faut pour cela definir une classe AlgoServer avec comme argument le cluster et la requete
    // le resultat de l'alogrithme renvoie le serveur adequat.

    AlgoServer algo1(cluster, request);
    Server     server_choosen;
    server_choosen = algo1.run();
    //server_choosen.print_server();

    // une fois le server choisi,
    // on lance l'algorithme qui permet de definir a quelle location du serveur la requete correspond
    //il faut pour cela definir une classe AlgoLocation avec comme argument le serveur et la requete.
    // le resultat de l'alogrithme renvoie la location adequate.
    AlgoLocation algo2(server_choosen, request);
    Location location_choosen = algo2.run();
    std::cout << "OKAY0" << std::endl;
    location_choosen.print_location();
    std::cout << "OKAY1" << std::endl;
    */
    /* BOUCLE DE N-SERVEUR NON BLOQUANTE*/
    Static_response   rsp_hdl;
    
    struct epoll_event event;
    std::memset(&event, 0, sizeof(epoll_event));
    while (42)
    {
            
            nb_fd_ready = epoll_wait(epoll_fd[number_server], event_wait[number_server], 32, 0);
            if (nb_fd_ready)
            {
                for (int i = 0; i < nb_fd_ready; i++) 
                {
                    int fd = event_wait[number_server][i].data.fd;

                    if ((event_wait[number_server][i].events & EPOLLIN) == EPOLLIN) //SI LE FD TRIGGER EPOLL POUR UNE LECTURE
                    {
                        if (fd == socket_server[number_server]) 
                        {
                            socket_client = accept(socket_server[number_server], (struct sockaddr*)&struct_client[number_server], &crecsize);
                            event.events  = EPOLLIN;
                            event.data.fd = socket_client;
                            epoll_ctl(epoll_fd[number_server], EPOLL_CTL_ADD, socket_client, &event);
                            
                        }
                        
                        else 
                        {
                            std::size_t bytes_received = recv(socket_client, buffer, sizeof(buffer), 0);
                            buffer[bytes_received] = 0;
                            if (bytes_received == 0)
                            {
                                std::cout << "Client closed the connection" << std::endl;
                                epoll_ctl(epoll_fd[number_server], EPOLL_CTL_DEL, socket_client, 0);
                                close (fd);                           
                            }
                            else
                            {

                                Request_parser  req_pars(buffer, fd); // fd
                                  
                                req_pars.parse_request();

                                rsp_hdl.setRequestParser(req_pars);

                                /*
                                rsp_hdl.upload_file();
                                */
                               
                                Request request(req_pars.getRequestMethod(), req_pars.getRequestHostname(), req_pars.getRequestUri(), "1.2.3.4", 13878);

                                AlgoServer algo1(cluster, request);
                                Server     server_choosen;
                                server_choosen = algo1.run();
                                //server_choosen.print_server();

                                AlgoLocation algo2(server_choosen, request);
                                Location location_chosen = algo2.run();

                                //location_chosen.print_location();
                                event.events = EPOLLOUT;
                                event.data.fd = socket_client;
                                epoll_ctl(epoll_fd[number_server], EPOLL_CTL_MOD, socket_client, &event); 
                                handle_request_execution(req_pars, location_chosen, rsp_hdl, fd);
                            }
                        }
                        
                    }
                    
                    
                    if ((event_wait[number_server][i].events & EPOLLOUT) == EPOLLOUT) //SI LE FD TRIGGER EPOLL POUR UNE ECRITURE
                    {

                        int count = send(socket_client, rsp_hdl.getBuffer(), rsp_hdl.getLength(), 0);

                        event.events = EPOLLIN;
                        event.data.fd = socket_client;
                        epoll_ctl(epoll_fd[number_server], EPOLL_CTL_MOD, socket_client, &event);

                    }
                    
                }
            }
            number_server++;
            if (number_server == number_server_max)
                number_server = 0;
            
    }
    
    return (0);
}

// BIZARRE LE UBUNTU_TESTER SEMBLE CONSIDERER QUE SI LA METHOD N'EST PAS ALLOWED ALORS AUCUNE REPONSE NE DEVRAIT ETRE ENVOYEE
// SINON RENVOIE UNSOLLICITED RESPONSE SI ON ESSAIE PAR EXEMPLE D'ENVOYER UNE PAGE D'ERREUR

// POUR UPLOAD:
// LE BOUNDARY COMMENCE TJRS PAR \r\n
// LA FIN DU BODY EST MARQUEE PAR LE DEBUT DU BOUNDARY SUIVANT, TOUT CE QUI EST AVANT FAIT PARTIE DU BODY
// DU COUP LE BODY EST PARSE AU FUR ET A MESURE, D'ABORD LE PREMIER BODY ENSUITE LE DEUXIEME...
// CE QUI SUIT LE DERNIER CHARACTER DU BOUNDARY NE COMPTE PAS POUR LE PARSING
// UN BOUNDARY COMMENCE TOUJOURS AU MINIMUM PAR UN SAUT DE LIGNE

// IL FAUT QU'UN BOUNDARY DE DEBUT EXACT SOIT TROUVE