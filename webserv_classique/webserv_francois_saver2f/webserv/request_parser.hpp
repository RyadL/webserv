#ifndef REQUEST_PARSER
# define REQUEST_PARSER

#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstring>
#include "../client_buffer.hpp"

class Request_parser
{
private:
    const char    **_reserved_meta_variables;
    std::string _request_header;
    const char  *_header_buffer;

    const char  *_body_buffer;
    int _fd;
    int _host_count;
    
    const char **ptr;
    const char  *_request_body_char;

    std::string _root_directory;
    std::string _index;
    std::string _title;
    std::string _request_hostname;
    std::string _request_method;
    std::string _request_uri;
    std::string _request_script_name;
    std::string _request_path_info;
    std::string _request_query_string;
    std::string _request_http_version;
    std::string _request_content_type;
    std::string _request_content_length;
    //std::string _request_user;
    std::vector<std::pair<std::string, std::string> >   _request_headers;
    std::vector<std::pair<std::string, std::string> >*  _request_headers_ptr;


    int     check_request_header_field_name(std::string, std::size_t);
    int     check_reserved_meta_variable(std::string, std::string);
    void    check_fatal_duplicate_meta_variable(std::string, std::string);
    void    check_duplicate_meta_variable(std::string);
    void    parse_request_line(std::string);
    void    parse_request_uri();
    void    parse_request_header(std::string);
    void    parse_request_body(std::string);
    void    parse_request_php_cgi();
    void    complete_meta_variables();
public:
    //Request_parser(std::string);
    Request_parser();
    //Request_parser(const char *, int);
    Request_parser(client_buffer*, int);
    Request_parser(const Request_parser& src);
    Request_parser& operator=(const Request_parser& other);

    ~Request_parser();
    void    parse_request();
    void    upload_file();
    void    swap(Request_parser&, Request_parser&);
    std::string getRequestMethod() const;
    std::string getRequestHostname() const;
    std::string getRequestUri() const;
    std::string getRequestScriptName() const;
    std::string getRequestQueryString() const;
    std::string getRequestPathInfo() const;
    std::string getRequestContentType() const;
    std::string getRequestContentLength() const;
    std::vector<std::pair<std::string, std::string> >*   getRequestHeadersPtr() const;
    std::string getRequestBody() const;
    const char* getRequestBodyChar() const;
    const char* getRequestHeaderBuffer() const;
    const char** getRequestPtr() const;
    std::string getRootDirectory() const;
    void    setRootDirectory(std::string);
    std::string getIndex() const;
    void    setIndex(std::string);
    std::string getTitle() const;
    void    setTitle(std::string);

    const char* getBodyBuffer() const;
    //
    void setRequestMethod(std::string method);
    //

    std::vector<std::pair<std::string, std::string> >   getRequestHeaders();
};

// _request_headers_ptr INUTILE, ON DOIT RETOURNER DIRECTEMENT _request_headers

#endif
