#ifndef STATIC_RESPONSE_HPP
# define STATIC_RESPONSE_HPP
// RESPONSE_HANDLER
#include "webserv.hpp"

/*
# include <vector>
# include <utility>
# include <string>
# include <iostream>
# include <fstream>
#include <limits>
#include <unistd.h>
#include <sys/socket.h>
# include "request_parser.hpp"
# include "MultipartParser.hpp"
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <map>
#include "Response_buffers.hpp"
*/

class   Static_response
{
private:
    
    Request_parser  _req_pars;

    std::map<int, Response_buffers*> _rsp_buf_tree;
    std::map<int, Response_buffers*> *_rsp_buf_tree_ptr;
    //Response_buffers    _rsp_buf;
    bool    _allowed_method;
    // STATUS LINE
    std::string _http_version;
    std::string _status_code;
    std::string _reason_phrase;
    int _fd;
    char  *_buffer;
    std::streamsize _length;

    std::string _boundary;
    std::size_t _header_end;

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
    std::size_t find_end_header_field(Response_buffers*, std::size_t, std::size_t, std::size_t);
    char*    generate_header_field(Response_buffers*, std::size_t*, std::size_t*, std::size_t);
    std::pair<std::size_t, std::size_t>    fill_buf_header(char *, char *, std::size_t*, std::pair<std::size_t, std::size_t>);
    std::pair<std::size_t, std::size_t>    fill_buf_body(char *, char *, std::size_t*, std::pair<std::size_t, std::size_t>);
    void    fill_buf_body_static(std::ifstream*, std::size_t, std::size_t);
    void    fill_tmpfile(std::pair<Response_buffers*, Response_buffers*>, std::size_t, std::size_t, std::size_t);
    void    add_cgi_header(std::size_t, std::size_t, Response_buffers*, Response_buffers*);
    void    check_fix_cgi_header(Response_buffers*, Response_buffers*);
    std::size_t    add_cgi_length(Response_buffers *, Response_buffers *);
    void    add_cgi_body(std::size_t, std::size_t, Response_buffers*, Response_buffers*);
    std::string    clean_upload_filename(std::string);
    //void    print_response_header(std::vector<std::pair<char*, std::size_t> > *, FILE*,  std::size_t);
    //int primary_check(std::size_t);
    //int further_validity_checks(std::size_t);

public:

    Static_response();
    Static_response(const Static_response&);
    Static_response(Request_parser, int);
    ~Static_response();


    void    add_client(int);
    std::string getStatusCode();
    void    setStatusCode(const char*);

    std::string getReasonPhrase();
    void    setReasonPhrase(const char*);

    void    create_response_header();

    void    execute_request();
    void    execute_uploading(std::vector<MultipartPart>, const char*);
    void    open_file_and_write(std::string, const char*, std::size_t);
    void    upload_file();

    void    prepare_cgi_response(Response_buffers*);

    void    setRequestParser(Request_parser);
    void    send_response();
    void    error_response(int);

    int getFd();
    void    setFd(int);

    char *getBuffer() const;

    std::streamsize getLength() const;

    Request_parser  getReqpars() const;

    std::string getBoundary() const;
    void    setAllowedMethod(bool);
    bool    getAllowedMethods() const;
    std::map<int, Response_buffers*> getBufTree() const;
    std::map<int, Response_buffers*> *getBufTreePtr() const;

    void    print_response_header(std::vector<std::pair<char*, std::size_t> > *, FILE*,  std::size_t);
};

#endif