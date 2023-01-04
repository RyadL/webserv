#ifndef HTTPKAWAIIREQUEST_HPP
# define HTTPKAWAIIREQUEST_HPP

#include <assert.h>     /* assert */
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <cerrno>
#include <sys/epoll.h>


#include <iostream>
#include <string>
#include <vector>
#include <sstream>

class HttpKawaiiRequest
{
private:

    struct epoll_event _event;
    int _client_socket_fd;
    int _epoll_fd;

    std::string _method;
    std::string _uri;
    std::string _http_version;
    std::string _host_header;

    //const char* _response;
    char    _response[1024];
    std::string _status_line;
    std::string _status_code;
    std::string _reason_phrase;
    std::vector<std::string>   _headers;

    const char* _body;
    std::size_t _body_length;

    const char* _raw_request;
    std::size_t _raw_request_len;

    const char* _raw_header;
    std::size_t _raw_header_len;

    const char* _raw_body;
    std::size_t _raw_body_len;   

public:
    HttpKawaiiRequest();
    ~HttpKawaiiRequest();
    void    kawaiiConnection();
    void    getRequest(std::string);
    void    addHeader(std::string);
    void    addBody(const char*);
    void    craftRawRequest(const char*);
    void    craftRawHeader(const char*);
    void    craftRawBody(const char*);
    void    sendRequest();
    void    sendRawRequest();
    void    sendRawHeaderBody();
    const char*   getResponse();
    void    receiveResponse();  

    class   SocketCreationFailure: public std::exception
    {
    public:
        const char*   what()
        {
            return "SocketCreationFailure exception";
        }
    };

    class   InetPtonFailure: public std::exception
    {
    public:
        const char*   what()
        {
            return "InetPtonFailure exception";
        }
    };

    class   ConnectionFailure: public std::exception
    {
    public:
        const char*   what()
        {
            return "ConnectionFailure exception";
        }
    };

    class   EpollFailure: public std::exception
    {
    public:
        const char*   what()
        {
            return "EpollFailure exception";
        }
    };

    class   ReceptionFailure: public std::exception
    {
    public:
        const char*   what()
        {
            return "ReceptionFailure exception";
        }
    };
};





#endif