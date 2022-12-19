#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "includes/webserv.hpp"


class   Request
{
    public:
        Request(){};
        Request(std::string method, std::string host, std::string path, std::string ip, int port):
            _method(method), _host(host), _path(path), _ip(ip), _port(port){}
        Request(const Request & A){*this = A;}
        ~Request(){}

        Request &   operator=(const Request & A)
        {
            _method = A._method;
            _host = A._host;
            _path = A._path;
            _ip = A._ip;
            _port = A._port;
            return (*this);
        }

        //getters
        std::string     get_method() const {return _method;}
        std::string     get_host() const {return _host;}
        std::string     get_path() const {return _path;}
        std::string     get_ip() const {return _ip;}
        int             get_port() const {return _port;}

        void    print()
        {
            std::cout << "method : " << _method << std::endl;
            std::cout << "host : " << _host << std::endl;
            std::cout << "path : " << _path << std::endl;
            std::cout << "ip : " << _ip << std::endl;
            std::cout << "port : " << _port << std::endl;
        }


    private:
        std::string                 _method;
        std::string                 _path;
        std::string                 _host;
        std::string                 _ip;
        int                         _port;


};

#endif