#include "httpKawaiiRequest.hpp"

#define IP_ADDRESS  "127.0.0.1"
#define SERV_PORT 13879

HttpKawaiiRequest::HttpKawaiiRequest()
    :_http_version("HTTP/1.1"), _raw_request(), _raw_header(), _raw_body(), _body()
{
    std::cout << "HttpKawaiiRequest default constructor" << std::endl;

}

void    HttpKawaiiRequest::kawaiiConnection()
{
    struct sockaddr_in servaddr;

    _client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_client_socket_fd == -1)
        throw SocketCreationFailure();
    std::memset(&servaddr, 0, sizeof(sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    if (inet_pton(AF_INET, IP_ADDRESS, &servaddr.sin_addr) == -1 && errno == EAFNOSUPPORT)
    {
        close(_client_socket_fd);
        throw InetPtonFailure();
    }
    if (connect(_client_socket_fd, (struct sockaddr*) &servaddr, sizeof(sockaddr_in)) == -1)
    {
        close(_client_socket_fd);
        throw ConnectionFailure();
    }
    /*
    _epoll_fd = epoll_create(0);
    if (_epoll_fd == -1)
    {
        close(_client_socket_fd);
        throw EpollFailure();
    }
    _event.events = EPOLLIN | EPOLLOUT;
    _event.data.fd = _client_socket_fd;
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _client_socket_fd, &_event))
    {
        close(_client_socket_fd);
        close(_epoll_fd);
        throw EpollFailure();
    }
    */
}

HttpKawaiiRequest::~HttpKawaiiRequest()
{
    std::cout << "HttpKawaiiRequest default destructor" << std::endl;
    if (_raw_request)
        delete [] _raw_request;
    if (_raw_header)
        delete [] _raw_header;
    if (_raw_body)
        delete [] _raw_body;
    if (_body)
        delete [] _body;
}

void    HttpKawaiiRequest::getRequest(std::string uri)
{
    _method = "GET";
    _uri = uri;
    // FAIRE CA MIEUX POUR IP_ADDRESS
    std::string ip_address(IP_ADDRESS);
    //ip_address += "\r\n";
    try
    {
        _headers.push_back("Host: " + ip_address);
    }
    catch(const std::exception& e)
    {
        close(_client_socket_fd);
        std::cerr << e.what() << '\n';
    }
    
    
}

void    HttpKawaiiRequest::addHeader(std::string header)
{
    try
    {
        _headers.push_back(header);
    }
    catch(const std::exception& e)
    {
        close(_client_socket_fd);
        std::cerr << e.what() << '\n';
    }
    
    
}

void    HttpKawaiiRequest::addBody(const char* body)
{
    std::stringstream ss;

    _body_length = std::strlen(body);
    try
    {
        _body = new char[_body_length];
    }
    catch(const std::exception& e)
    {
        close(_client_socket_fd);
        std::cerr << e.what() << '\n';
    }
    
    std::memmove((void*)_body, body, _body_length);

    ss << _body_length;
    std::string content_length = ss.str();
    content_length = "Content-Length: " + content_length;
    try
    {
        _headers.push_back(content_length);
    }
    catch(const std::exception& e)
    {
        close(_client_socket_fd);
        delete [] _body;
        std::cerr << e.what() << '\n';
    }
    

}
// POUR LES BODIES BINAIRES ON VA LIRE LES FICHIERS DANS LE BODY DIRECTEMENT

void    HttpKawaiiRequest::craftRawRequest(const char *raw_request)
{
    _raw_request_len = std::strlen(raw_request);
    try
    {
        _raw_request = new char[_raw_request_len];
    }
    catch(const std::exception& e)
    {
        close(_client_socket_fd);
        std::cerr << e.what() << '\n';
    }
    std::memcpy((void*)_raw_request, raw_request, _raw_request_len);
}

void    HttpKawaiiRequest::craftRawHeader(const char* raw_header)
{
    _raw_header_len = std::strlen(raw_header);
    try
    {
        _raw_header = new char[_raw_header_len + 2];
    }
    catch(const std::exception& e)
    {
        close(_client_socket_fd);
        std::cerr << e.what() << '\n';
    }
    std::memcpy((void*)_raw_header, raw_header, _raw_header_len);
    std::memcpy((void*)(_raw_header + _raw_header_len), "\r\n", 2);
    _raw_header_len += 2;
}

void    HttpKawaiiRequest::craftRawBody(const char *raw_body)
{
    _raw_body_len = std::strlen(raw_body);
    try
    {
        _raw_body = new char[_raw_body_len];
    }
    catch(const std::exception& e)
    {
        close(_client_socket_fd);
        std::cerr << e.what() << '\n';
    }
    std::memcpy((void*)_raw_body, raw_body, _raw_body_len);
}

/*
void    HttpKawaiiRequest::craftBodyFromFile(int fd)
{
    
}
*/

void    HttpKawaiiRequest::sendRawRequest()
{
    //std::cout << "RAW_REQUEST667:\n" << std::endl;
    //std::cout.write(_raw_request, _raw_request_len);
    //std::cout << "END" << std::endl;
    send(_client_socket_fd, _raw_request, _raw_request_len, 0);
}

void    HttpKawaiiRequest::sendRawHeaderBody()
{
    const char *tmp;
    try
    {
        tmp = new char[_raw_header_len + _raw_body_len];
    }
    catch(const std::exception& e)
    {
        close(_client_socket_fd);
        std::cerr << e.what() << '\n';
    }
    std::memcpy((void*)tmp, _raw_header, _raw_header_len);
    std::memcpy((void*)(tmp + _raw_header_len), _raw_body, _raw_body_len);
    send(_client_socket_fd, tmp, _raw_header_len + _raw_body_len, 0);
    delete [] tmp;
}

void    HttpKawaiiRequest::sendRequest()
{
    const char* request;
    std::size_t header_length;

    header_length = 0;
    std::string request_line(_method + " " + _uri + " " + _http_version + "\r\n");
    std::string headers;

    std::vector<std::string>    tmp = _headers;
    for (std::vector<std::string>::const_iterator   it = tmp.begin(); it != tmp.end(); it++)
    {
        headers += *it;
        headers += "\r\n";
        
    }
    headers += "\r\n";
    try
    {
        request = new char[request_line.length() + _host_header.length() + headers.length() + _body_length];
    }
    catch(const std::exception& e)
    {
        close(_client_socket_fd);
        
        std::cerr << e.what() << '\n';
    }
    std::memmove((void*)request, (const void*)request_line.c_str(), request_line.length());
    std::memmove((void*)(request + request_line.length()), (const void*)_host_header.c_str(), _host_header.length());
    std::memmove((void*)(request + request_line.length() + _host_header.length()), (const void*)headers.c_str(), headers.length());
    std::memmove((void*)(request + request_line.length() + _host_header.length() + headers.length()), (const void*)_body, _body_length);
    send(_client_socket_fd, request, request_line.length() + _host_header.length() + headers.length() + _body_length, 0);
    delete [] request;
}

const char*   HttpKawaiiRequest::getResponse()
{
    return _response;
}

void    HttpKawaiiRequest::receiveResponse()
{
    if (recv(_client_socket_fd, _response, 1024, 0) == -1)
        throw ReceptionFailure();
}