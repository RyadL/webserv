#ifndef STATIC_RESPONSE
# define STATIC_RESPONSE
// RESPONSE_HANDLER
# include <vector>
# include <utility>
# include <string>
# include <iostream>
# include <fstream>
#include <limits>
#include <unistd.h>
#include <sys/socket.h>
# include "../webserv/request_parser.hpp"
# include "../response/MultipartParser.hpp"
#include <sstream>
#include <fstream>
#include <sys/stat.h>

class   Static_response
{
private:
    Request_parser  _req_pars;
    bool    _allowed_method;
    // STATUS LINE
    std::string _http_version;
    std::string _status_code;
    std::string _reason_phrase;
    int _fd;
    char  *_buffer;
    std::streamsize _length;

    std::string _boundary;

    // HEADERS
    std::vector<std::pair<std::string, std::string> >   _response_headers;
    //std::vector<std::pair<std::string, std::string> >*  _response_headers_ptr;
    // MESSAGE BODY
    //create_response_header_field();
    std::ptrdiff_t    check_multipart_header();
    int retrieve_boundary(std::string);
    void    find_part();
    std::size_t parse_part(std::size_t);
    std::size_t    find_boundary_start(std::size_t, int);
    std::size_t    find_boundary_end(std::size_t);
    int check_final_boundary(std::size_t);
    //int primary_check(std::size_t);
    //int further_validity_checks(std::size_t);

public:
    Static_response();
    Static_response(Request_parser, int);
    ~Static_response();

    std::string getStatusCode();
    void    setStatusCode(const char*);

    std::string getReasonPhrase();
    void    setReasonPhrase(const char*);

    void    create_response_header();

    void    execute_request();
    void    execute_uploading(std::vector<MultipartPart>, const char*);
    void    open_file_and_write(std::string, const char*, std::size_t);
    void    upload_file();

    void    prepare_cgi_response(char *, std::size_t);

    void    setRequestParser(Request_parser);

    void    error_response(int);

    int getFd();
    void    setFd(int);

    char *getBuffer() const;

    std::streamsize getLength() const;

    Request_parser  getReqpars() const;

    std::string getBoundary() const;
    void    setAllowedMethod(bool);
    bool    getAllowedMethods() const;

};

#endif