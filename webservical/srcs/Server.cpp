#include "../includes/webserv.hpp"


Server::Server():_name(""),_ip("0.0.0.0"),_port(80),_clientBodySize(16000)
{
    std::vector<std::string> methods;
    methods.push_back("GET");
    methods.push_back("POST");
    methods.push_back("DELETE");
    _allowedMethods = methods;
}
Server::Server(const Server & A){*this = A;}
Server::~Server(){}
Server &   Server::operator=(const Server & A)
{
    _name = A._name;
    _ip = A._ip;
    _port = A._port;
    _root = A._root;
    _index = A._index;
    _errorPages = A._errorPages;
    _clientBodySize = A._clientBodySize;
    _allowedMethods = A._allowedMethods;
    _redirection = A._redirection;
    _locations = A._locations;
    _autoIndex = A._autoIndex;
    return (*this);
}



void            Server::parse_error_pages(std::vector<std::string> lineVector)
{
    int             key;
    std::string     value;

    std::vector<int> tab;

    for (int i = 1; i < lineVector.size(); i++)
    {
        if (lineVector[i][0] != '/')
        {
            const char *c = lineVector[i].c_str();
            key = std::atoi(c);
            tab.push_back(key);
        }
        else
        {
            value = lineVector[i];
            value.pop_back();
        }
    }
    for (int i = 0; i < tab.size(); i++)
    {
        std::pair<int, std::string>   p(tab[i], value);
        _errorPages.insert(p);
    }
}

void            Server::parse_client_body_size(std::vector<std::string> lineVector)
{
    const char *c = lineVector[1].c_str();
    _clientBodySize = std::atoi(c) * 1000;
}

void            Server::parse_allow_methods(std::vector<std::string> lineVector)
{

    _allowedMethods.clear();
    for (int i = 1; i < lineVector.size(); i++)
    { 
      _allowedMethods.push_back(lineVector[i]);
    }
    _allowedMethods[_allowedMethods.size() - 1].pop_back();
}

unsigned int   Server::parse_location(std::vector<std::string> fileVector,unsigned int i)
{
    Location    location;

    std::vector<std::string>    lineVector;    
    lineVector = split(fileVector[i]);

    if (lineVector.size() == 4)
    {
        location.set_prefix(lineVector[1]);
        location.set_title(lineVector[2]);
    }
    else if (lineVector.size() == 3)
    {
        location.set_prefix("");
        location.set_title(lineVector[1]);
    }
    else
    {
        std::cout << "error in the configuration location\n";
        exit (1);
    }
    i++;
    i = location.parse(fileVector, i);
    _locations.push_back(location);
    return i;
}

unsigned int    Server::parse(std::vector<std::string> fileVector, unsigned int i)
{
    while (fileVector[i] != "}")
    {
        std::vector<std::string>    lineVector;
        lineVector = split(fileVector[i]);

        if (lineVector[0] == "server_name")
        {
            lineVector[1].pop_back();
            _name = lineVector[1];
        }
        else if (lineVector[0] == "listen")
        {
            std::vector<std::string> listen;
            listen = split2(lineVector[1], ':');
            if ((listen.size() == 1) && (listen[0].find('.') == -1))
            {
                const char *c = listen[0].c_str();
                _port = std::atoi(c);
            }
            else if ((listen.size() == 1) && (listen[0].find('.') != -1))
                _ip = listen[0]; 
            else
            {
                _ip = listen[0];
                const char *c = listen[1].c_str();
                _port = std::atoi(c);
            }
        }
        else if (lineVector[0] == "root")
        {
            lineVector[1].pop_back();
            _root = lineVector[1];
        }
        else if (lineVector[0] == "index")
        {
            lineVector[1].pop_back();
            _index = lineVector[1];
        }
        else if (lineVector[0] == "error_page")
            parse_error_pages(lineVector);
        else if (lineVector[0] == "client_body_buffer_size")
            parse_client_body_size(lineVector);
        else if (lineVector[0] == "allow_methods")
            parse_allow_methods(lineVector);
        else if (lineVector[0] == "return")
        {
            lineVector[2].pop_back();
            _redirection = lineVector[2];
        }
        else if (lineVector[0] == "location")
        {
            i = parse_location(fileVector, i);
            //lineVector = split(fileVector[i]);
        }
        else if (lineVector[0] == "autoindex")
        {
            lineVector[1].pop_back();
            _autoIndex = lineVector[1];
        }
            
        else 
        {
            if (lineVector[0] != "\n")
            {
                std::cout << "error in the configuration file dodo: " << lineVector[0] << std::endl;
                return (0);
            }
        }
        i++;
    }
    return i;
}

void    Server::print_server()
{
    std::cout << "serveur name : " << _name << std::endl;
    std::cout << "ip       : " << _ip << std::endl;
    std::cout << "port       : " << _port << std::endl;
    std::cout << "root      : " << _root << std::endl;
    std::cout << "index      : " << _index << std::endl;
    
   for (std::map<int,std::string>::iterator it = _errorPages.begin(); it != _errorPages.end(); it++)
    {
        std::cout << "error pages : number : " << it->first << std::endl;
        std::cout << "error_page : uri :"  << it->second << std::endl;
    }
    
    std::cout << "client_body_size : "  << _clientBodySize << std::endl;

    for (int i = 0; i < _allowedMethods.size(); i++)
        std::cout << "allow_methods : "  << _allowedMethods[i] << std::endl;

    std::cout << "redirection : "  << _redirection << std::endl;

    std::cout << "autoindex : "  << _autoIndex << std::endl;

    for (int i = 0; i < _locations.size(); i++)
        _locations[i].print_location();
}

        std::string                     Server::get_name() const {return _name;}
        int                             Server::get_port() const {return _port;};
        std::string                     Server::get_ip() const {return _ip;}
        std::vector<Location>           Server::get_locations() const {return _locations;}
