#ifndef CGI_HANDLER
# define CGI_HANDLER

#include "request_parser.hpp"
#include "../response/static_response.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <string>
#include <cstring>

class   Cgi_handler
{
private:
    Request_parser  _req_parser;
    //Static_response _rsp_hdl;

    char    *_buffer;
    std::size_t _length;
    //std::vector<std::string> _meta_variables;
    char *_meta_user;
    char    **_meta_env;
    //const char    **_reserved_meta_variables;
    std::vector<std::pair<std::string, std::string> >   _response_headers;

    void    execute_get_request();
    void    execute_post_request();
    void    execute_delete_request();
    void    create_meta_variables(std::string, std::string, int *);
    void    add_request_line_meta_variables(int *);
    void    add_header_meta_variables(int *);
    void    add_server_meta_variables(int *);
    int is_reserved_meta_variable(std::string);

    
public:
    Cgi_handler(Request_parser);
    void    execute_request();
    char    *getBuffer() const;
    std::size_t getLength() const;
};

#endif