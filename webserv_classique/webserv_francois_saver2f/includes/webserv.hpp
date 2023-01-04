#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <tuple>
#include <map>

#include "../Location.hpp"
#include "../Server.hpp"
#include "../Cluster.hpp"
#include "../Request.hpp"
#include "../algolocation.hpp"
#include "../AlgoServer.hpp"
#include "../webserv/request_parser.hpp"
#include "../webserv/cgi_handler.hpp"
#include "../response/static_response.hpp"
#include "../response/MultipartParser.hpp"
#include "../socket_class.hpp"
#include "../client_buffer.hpp"
#include <fcntl.h>


#include <algorithm>
#include <fcntl.h>
 #include <fcntl.h>
 #include <sys/types.h>
 #include <sys/stat.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <cstring>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
typedef struct sockaddr SOCKADDR;
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

//colors
# define RED "\033[31m"
# define GREEN "\033[32m"
# define YELLOW "\033[33m"
# define RESET "\033[0m"




std::vector<std::string>		split(const std::string& str);
std::vector<std::string>		split2(const std::string& str, char c);
int                             is_closed_chevron(std::string str);
void    handle_client_request(const char *);
int end_equality(std::string name, std::string requete_name);
int begin_equality(std::string name, std::string requete_name);
void    handle_request_execution(Request_parser& req_pars, Location& location_chosen, Static_response& rsp_hdl, int fd);
int    check_allowed_methods_request(std::string);



#endif