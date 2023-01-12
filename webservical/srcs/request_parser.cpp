#include "../includes/webserv.hpp"

Request_parser::Request_parser()
    :_reserved_meta_variables(NULL)
{
    std::cout << "Request_parser default constructor" << std::endl;
}

Request_parser::Request_parser(client_buffer* cl_buf,int fd)
    :_fd(fd), _request_headers_ptr(&_request_headers), _is_short_body(cl_buf->is_short_body),
    _is_long_body(cl_buf->is_long_body), _body_buffer(cl_buf->short_body), _bytes_inside_body(cl_buf->bytes_inside_body),
    _request_body_char(cl_buf->short_body), _fd_long_body(cl_buf->fd_long_body),
    _file_long_body(cl_buf->_file_long_body), _bytes_inside_file_body(cl_buf->size), _host_count(0), _body_buffer_ptr(&_body_buffer)
{
    std::cout << "Request_parser constructor" << std::endl;
    //std::cout << "START: " << cl_buf->long_header[0] << "END" << std::endl;
    if (cl_buf->is_short_header)
    {
        _request_header = std::string(cl_buf->short_header, 0, cl_buf->bytes_inside_short_header);
        //_header_buffer = cl_buf->short_header;
    }
    if (cl_buf->is_long_header > -1)
    {
        //std::cout << "OKAY0" << std::endl;
        for (int i = 0; cl_buf->long_header[i]; i++)
            _request_header += std::string(cl_buf->long_header[i]);
        //_header_buffer = 
    }
    const char *reserved_var[] = {
        "AUTH_TYPE", "REDIRECT_STATUS", "SERVER_NAME",
        "SERVER_PORT", "SERVER_ADDR", "REMOTE_PORT",
        "REMOTE_ADDR","SERVER_SOFTWARE", "GATEWAY_INTERFACE",
        "REQUEST_SCHEME", "SERVER_PROTOCOL", "DOCUMENT_ROOT",
        "DOCUMENT_URI", "REQUEST_URI", "SCRIPT_NAME",
        "REQUEST_METHOD", "QUERY_STRING", "SCRIPT_FILENAME",
        "PATH_INFO", "CGI_ROLE", "PHP_SELF",
        "REQUEST_TIME_FLOAT", "REQUEST_TIME"
    };
    _reserved_meta_variables = new const char*[22];
    for (int i = 0; i < 22; i++)
        _reserved_meta_variables[i] = reserved_var[i];
    std::cout << "_REQUEST_HEADER:" << std::endl;
    std::cout.write(_request_header.c_str(), cl_buf->bytes_inside_short_header) << "END" << std::endl;
    std::cout << "_REQUEST_HEADER:" << std::endl;
    std::cout << _request_header << "END" << std::endl;
   
    std::cout << "_BODY_BUFFER:" << std::endl;
    std::cout.write(_body_buffer, _bytes_inside_body) << "END" << std::endl;
    std::cout << "BYTES_NSIDE_BODY: " << _bytes_inside_body << std::endl;
    
    /*
    char    testbuf[4096];
    std::size_t ret = 1;


    std::cout << "My lenght = " << _bytes_inside_file_body << "of = " << fd1 << std::endl;
   
    
    while (ret)
    {
        ret = read(fd1, testbuf, 4096);
        std::cout << "ret: " << ret << std::endl;
        write(1, testbuf, ret);
    }
    
    //write(fileno(_file_long_body) )

    //std::cout << "_is_long_body: " << _is_long_body << std::endl;
    //std::cout << "cl_buf->_bytes_inside_file_body: " << cl_buf->_bytes_inside_file_body << std::endl;
    //std::cout << "_bytes_inside_file_body: " << _bytes_inside_file_body << std::endl;
    */
    
}

// ON A REMPLACER getRequestBodyChar()) PAR _request_body_char
Request_parser::Request_parser(const Request_parser& src)
    :_request_header(src._request_header), _request_method(src._request_method), _request_uri(src._request_uri),
    _request_script_name(src._request_script_name), _request_path_info(src._request_path_info), _request_query_string(src._request_query_string),
    _request_http_version(src._request_http_version), _request_content_type(src._request_content_type), _request_content_length(src._request_content_length),
    _request_headers(src._request_headers), _host_count(src._host_count), _header_buffer(src._header_buffer),
    _request_body_char(src._request_body_char), _root_directory(src._root_directory), _index(src._index),
    _title(src._title), _request_script_filename(src._request_script_filename), _is_short_body(src._is_short_body),
    _is_long_body(src._is_long_body), _body_buffer(src._body_buffer), _fd_long_body(src._fd_long_body),
    _file_long_body(src._file_long_body), _bytes_inside_body(src._bytes_inside_body), _bytes_inside_file_body(src._bytes_inside_file_body)
{
    std::cout << "REQUEST_PARSER COPY CONSTRUCTOR" << std::endl;

    _request_headers_ptr = &_request_headers;
    _body_buffer_ptr = &_body_buffer;
    /*
    if (_is_short_body)
    {
        _body_buffer = new char[16 * 1024];
        std::memcpy((void*)_body_buffer, src._body_buffer, src._bytes_inside_body);
    }
    */
    std::cout.write(*_body_buffer_ptr, _bytes_inside_body) << "END667974" << std::endl;
    //for (std::vector<std::pair<std::string, std::string>>::iterator it = _request_headers.begin(); it != _request_headers.end(); it++)
    //    std::cout << "it->first: " << it->first << std::endl;
    const char *reserved_var[] = {
        "AUTH_TYPE", "REDIRECT_STATUS", "SERVER_NAME",
        "SERVER_PORT", "SERVER_ADDR", "REMOTE_PORT",
        "REMOTE_ADDR","SERVER_SOFTWARE", "GATEWAY_INTERFACE",
        "REQUEST_SCHEME", "SERVER_PROTOCOL", "DOCUMENT_ROOT",
        "DOCUMENT_URI", "REQUEST_URI", "SCRIPT_NAME",
        "REQUEST_METHOD", "QUERY_STRING", "SCRIPT_FILENAME",
        "PATH_INFO", "CGI_ROLE", "PHP_SELF",
        "REQUEST_TIME_FLOAT", "REQUEST_TIME"
    };
    //std::cout << "END COPY" << std::endl;
    //std::cout << "_title667: " << _title << std::endl;
    _reserved_meta_variables = new const char*[22];
    for (int i = 0; i < 22; i++)
        _reserved_meta_variables[i] = reserved_var[i];
}

Request_parser& Request_parser::operator=(const Request_parser& other)
{
    std::cout << "REQUEST_PARSER COPY ASSIGNMENT" << std::endl;
    Request_parser  tmp(other);
    //exit(0);
    swap(*this, tmp);
    //exit(0);
    //std::cout << "TEST0: " << getRequestUri() << std::endl;
    //std::cout << "TEST1: " << getRootDirectory() << std::endl;
    //std::cout << "TEST2: " << getIndex() << std::endl;
    //std::cout << "_title974: " << _title << std::endl;
    return *this;
}

Request_parser::~Request_parser()
{
    //std::cout << "Request_parser destructor" << std::endl;
    //if (_reserved_meta_variables)
    //{
    //    std::cout << "ON EST LA" << std::endl;
        delete[] _reserved_meta_variables;
    //}
        
}

void    Request_parser::parse_request_line(std::string line)
{
    std::size_t found0 = _request_header.find("\r\n");
    if (found0 == std::string::npos)
        std::cout << "NOPE" << std::endl;

    std::size_t found1 = line.find(" ");
    _request_method = line.substr(0, found1);
    if (found1 == std::string::npos)
    {
        std::cout << "ERROR0" << std::endl;
        return;
    }
    /*
    if (!(_request_method == "GET" || _request_method == "POST" || _request_method == "DELETE"))
    {
        std::cout << "ERROR1" << std::endl;
        return;
    }
    */
    for (; !line.compare(found1, 1, " "); found1++);
    std::size_t found2 = line.find(" ", found1);
    if (found2 == std::string::npos)
    {
        std::cout << "ERROR2" << std::endl;
        return;
    }
    _request_uri = line.substr(found1, found2 - found1);
    for (; !line.compare(found2, 1, " "); found2++);
    if (found2 == line.length())
    {
        std::cout << "YAYAYA" << std::endl;
        return;
    }
        
    std::size_t found3 = line.find(" ", found2);
    _request_http_version = line.substr(found2, found3 - found2);
    if (_request_http_version[0] != 'H')
    {
        //std::cout << "ERROR NO RESPONSE" << std::endl;
        return;
    }
    if (_request_http_version.compare(0, 8, "HTTP/1.1") != 0)
    {
        std::cout << "ERROR3" << std::endl;
        return;
    }
    std::size_t    it = found2 + 8;
    for (; !line.compare(it, 1, " "); it++);
    if (it != line.length())
    {
        std::cout << "ERROR4" << std::endl;
        return;
    }
        
}

void    Request_parser::parse_request_php_cgi()
{
    std::size_t found1 = _request_uri.find("?");
    if (found1 != std::string::npos)
    {
        _request_query_string = _request_uri.substr(found1 + 1, std::string::npos);
        //std::cout << "QUERY STRING: " << _request_query_string << std::endl;
        
    }
    _request_script_name = _request_uri.substr(0, found1);
    std::size_t found2 = 0;
    std::cout << "SCRIPT_NAME0: " << _request_script_name << std::endl;
    std::cout << "PATH_INFO0: " << _request_path_info << std::endl;
    while (found2 != std::string::npos)  // CAR UN DOSSIER PEUT ETRE EN .PHP ETC... ET ON CHERCHE LA PREMIERE OCCURENCE DE .PHP
    {
        //
        found2 = _request_uri.find(".php", found2);
        if (found2 == std::string::npos)
        {
            std::cout << "ERROR5" << std::endl;
            return ;
        }
        found2 += 4;
        if (found2 == _request_uri.length() || !_request_uri.compare(found2, 1, "/") || !_request_uri.compare(found2, 1, "?"))
        {

    
            _request_script_name = _request_uri.substr(0, found2);
            _request_path_info = _request_uri.substr(found2, found1 - found2);
            std::cout << "SCRIPT_NAME1: " << _request_script_name << std::endl;
            std::cout << "PATH_INFO1: " << _request_path_info << std::endl;
            return ;
        }
            
    }
}


/*
void    Request_parser::parse_request_uri()
{
    
    std::cout << "getRequestScriptName()1: " << getRequestScriptName() << std::endl;
    parse_request_php_cgi();
    std::cout << "getRequestScriptName()2: " << getRequestScriptName() << std::endl;
    
    
}
*/


void    Request_parser::parse_request_uri()
{
    std::size_t found1 = _request_uri.find("?");
    if (found1 != std::string::npos)
    {
        _request_query_string = _request_uri.substr(found1 + 1, std::string::npos);
        //std::cout << "QUERY STRING: " << _request_query_string << std::endl;
        
    }
    _request_script_name = _request_uri.substr(0, found1);
    _request_query_string_start = found1;
    std::cout << "SCRIPT_NAME0: " << _request_script_name << std::endl;
    std::cout << "QUERY_STRING0: " << _request_query_string << std::endl;  
}


int Request_parser::check_reserved_meta_variable(std::string var, std::string value)
{

    for (int i = 0;  i < 22; i++)
    {

        if (var == "CONTENT_TYPE")
        {
            _request_content_type = value;
            return 0;
        }
        if (var == "CONTENT_LENGTH")
        {
            _request_content_length = value;
            return 0;
        }

        if (var.find(_reserved_meta_variables[i]) != std::string::npos)
        {
            //std::cout << "RESERVED" << std::endl;
            //std::cout << "_reserved_meta_variables[i]: " << _reserved_meta_variables[i] << " et var.c_str(): " << var.c_str() << std::endl;
            return 1;
        }
            
    }
       
    return 0;
}

void    Request_parser::check_fatal_duplicate_meta_variable(std::string var, std::string value)
{
    if (var == "HOST" && value == "HOST")
    {
        //std::cout << "ERROR 667" << std::endl;
        exit (1);
    }
    if (var == "CONTENT_LENGTH" && value == "CONTENT_LENGTH")
    {
        //std::cout << "ERROR 667" << std::endl;
        exit (1);
    }
}

void    Request_parser::check_duplicate_meta_variable(std::string var)
{
    for (std::vector<std::pair<std::string, std::string> >::iterator    it = _request_headers.begin(); it != _request_headers.end(); it++)
    {

        check_fatal_duplicate_meta_variable(var, it->first);

        if (it->first == var)
        {
            std::cout << "DUPLICATE: " << it->first << std::endl;
            _request_headers.erase(it);
            return;
        }
    }
}

int     Request_parser::check_request_header_field_name(std::string line, std::size_t found)
{
    for (std::size_t    i = 0; i < found; i++)
    {
        if (!(std::isalnum(line[i]) || line[i] == '-' ))
        {
            std::cout << "IGNORE: " << line.substr(0, found) << " et i: " << i << std::endl;
            return 1;
        }
            
    }
    return 0;
}

void    Request_parser::parse_request_header(std::string line)
{
    if (line.empty())
        std::cout << "ERROR6" << std::endl;
    std::size_t found1 = line.find(":");
    if (found1 == std::string::npos)
        found1 = line.length();

    std::string field_name = line.substr(0, found1);
    std::string field_value;
    if (found1 != line.length())
    {
        found1++;
        if (found1 != std::string::npos)
        {
            for (;  !line.compare(found1, 1, " "); found1++);
            field_value = line.substr(found1, line.length() - found1);
            
        }
        for (std::string::iterator  it = field_name.begin(); it != field_name.end(); it++)
        {
            if (*it >= 'a' && *it <= 'z')
                *it -= 32;
            if (*it == '-')
                *it = '_';
        }
        if (check_reserved_meta_variable(field_name, field_value))
            return ;
        check_duplicate_meta_variable(field_name);
        if (field_name == "HOST")
        {
            _host_count++;
            _request_hostname = field_value;
        }
            
        if (_host_count > 1)
        {
            std::cout << "HOST ERROR" << std::endl;
            return ;
        }
    }
    _request_headers.push_back(std::pair<std::string, std::string>(field_name, field_value));
    // IL FAUDRA CHECKER SI LE PARSING DE LA FIELD_VALUE EST SUFFISANT PAR EXEMPLE TRIMMER HOSTNAME
    // CHECKER LES WHITESPACES
}
//

//
void    Request_parser::parse_request()
{
    //std::cout << "REQUEST667: " << _request_header << std::endl;
    std::size_t found1 = _request_header.find("\r\n");
    if (found1 == std::string::npos)
        std::cout << "NOPE" << std::endl;
    
    
    parse_request_line(_request_header.substr(0, found1));
    //std::cout << "_request_uri: " << _request_uri << std::endl;
    /*std::cout << "_request_method: " << _request_method << std::endl;
    std::cout << "_request_uri: " << _request_uri << std::endl;
    std::cout << "_request_http_version: " << _request_http_version << std::endl;*/
    
    
    std::size_t found2 = 0;
    while (found2 != std::string::npos)
    {
        found1 += 2;
        found2 = _request_header.find("\r\n", found1);

        if (found1 == found2)
            break;
        parse_request_header(_request_header.substr(found1, found2 - found1));
        found1 = found2;  
    }
    if (_host_count != 1)
    {
        //std::cout << "ERROR HOST" << std::endl;
        return ;
    }
    
    //found1 += 2;
    //_request_body_char = _buffer + found1;

    std::cout << "getRequestScriptName()0: " << getRequestScriptName() << std::endl;
    parse_request_uri();
    std::cout << "getRequestScriptName()3: " << getRequestScriptName() << std::endl;
    
    //for (std::vector<std::pair<std::string, std::string> >::iterator it = _request_headers.begin(); it != _request_headers.end(); it++)
    //    std::cout << "it->first: " << it->first << " et it->second: " << it->second << std::endl;
    

}

void    Request_parser::parse_request_cgi(int cgi)
{
    std::size_t found2 = 0;
    const char* script_suf;

    if (cgi == 0)
        script_suf = ".php";
    if (cgi == 1)
        script_suf = ".py";
    while (found2 != std::string::npos)
    {
        found2 = _request_uri.find(script_suf, found2);
        if (found2 == std::string::npos)
        {
            std::cout << "ERROR5" << std::endl;
            _request_script_filename = _root_directory + _request_script_name;
            return ;
        }
        found2 += std::strlen(script_suf);
        if (found2 == _request_uri.length() || !_request_uri.compare(found2, 1, "/") || !_request_uri.compare(found2, 1, "?"))
        {

    
            _request_script_name = _request_uri.substr(0, found2);
            _request_path_info = _request_uri.substr(found2, _request_query_string_start - found2);
            std::cout << "SCRIPT_NAME1: " << _request_script_name << std::endl;
            std::cout << "PATH_INFO1: " << _request_path_info << std::endl;
            _request_script_filename = _root_directory + _request_script_name;
            std::cout << "SCRIPT_FILENAME1: " << _request_script_filename << std::endl;
            //exit(0);
            return ;
        }
            
    }
    //std::cout << "AIE" << std::endl;
    //exit(0);
}

void    Request_parser::swap(Request_parser& a, Request_parser& b)
{
    const char **save_reserved_meta_variables = a._reserved_meta_variables;
    std::string save_root_directoy = a._root_directory;
    std::string save_request_uri = a._request_uri;
    std::string save_index = a._index;
    const char  *save_request_body_char = a._request_body_char;
    std::vector<std::pair<std::string, std::string> >   save_request_headers = a._request_headers;
    std::vector<std::pair<std::string, std::string> >*  save_request_headers_ptr = a._request_headers_ptr;
    std::string save_title = a._title;
    std::size_t save_bytes_inside_body = a._bytes_inside_body;
    const char  *save_body_buffer = a._body_buffer;
    const char  **save_body_buffer_ptr = a._body_buffer_ptr;
    bool    save_is_short_body = a._is_short_body;
    bool    save_is_long_body = a._is_long_body;
    std::size_t save_bytes_inside_file_body = a._bytes_inside_file_body;
    FILE    *save_file_long_body = a._file_long_body;
    
    a._reserved_meta_variables = b._reserved_meta_variables;
    b._reserved_meta_variables = save_reserved_meta_variables;
    a._root_directory = b._root_directory;
    b._root_directory = save_root_directoy;
    a._request_uri = b._request_uri;
    b._request_uri = save_request_uri;
    a._index = b._index;
    b._index = save_index;
    a._request_body_char = b._request_body_char;
    b._request_body_char = save_request_body_char;
    a._request_headers = b._request_headers;
    b._request_headers = save_request_headers;
    a._request_headers_ptr = b._request_headers_ptr;
    b._request_headers_ptr = save_request_headers_ptr;
    a._title = b._title;
    b._title = save_title;
    a._bytes_inside_body = b._bytes_inside_body;
    b._bytes_inside_body = save_bytes_inside_body;
    a._body_buffer = b._body_buffer;
    b._body_buffer = save_body_buffer;
    a._body_buffer_ptr = b._body_buffer_ptr;
    b._body_buffer_ptr = save_body_buffer_ptr;
    a._is_short_body = b._is_short_body;
    b._is_short_body = save_is_short_body;
    a._is_long_body = b._is_long_body;
    b._is_long_body = save_is_long_body;
    a._bytes_inside_file_body = b._bytes_inside_file_body;
    b._bytes_inside_file_body = save_bytes_inside_file_body;
    a._file_long_body = b._file_long_body;
    save_file_long_body = a._file_long_body;
}

std::string Request_parser::getRequestMethod() const
{
    return _request_method;
}

// POUR TESTS UNIQUEMENT
void Request_parser::setRequestMethod(std::string method)
{
    _request_method = method;
}
//

std::string Request_parser::getRequestHostname() const
{
    return _request_hostname;
}

std::string Request_parser::getRequestUri() const
{
    return _request_uri;
}

std::string Request_parser::getRequestScriptName() const
{
    return _request_script_name;
}

std::string Request_parser::getRequestScriptFilename() const
{
    return _request_script_filename;
}

std::string Request_parser::getRequestQueryString() const
{
    return _request_query_string;
}

std::string Request_parser::getRequestPathInfo() const
{
    return _request_path_info;
}

std::vector<std::pair<std::string, std::string> >*   Request_parser::getRequestHeadersPtr() const
{
    return _request_headers_ptr;
}

std::string Request_parser::getRequestContentType() const
{
    return _request_content_type;
}

std::string Request_parser::getRequestContentLength() const
{
    return _request_content_length;
}


const char* Request_parser::getBodyBuffer() const
{
    return _body_buffer;
}

const char**    Request_parser::getBodyBufferPtr() const
{
    return _body_buffer_ptr;
}

const char* Request_parser::getRequestBodyChar() const
{
    return _request_body_char;
}

const char* Request_parser::getRequestHeaderBuffer() const
{
    return _header_buffer;
}

const char** Request_parser::getRequestPtr() const
{
    return ptr;
}

std::string Request_parser::getRootDirectory() const
{
    return _root_directory;
}

void    Request_parser::setRootDirectory(std::string root_directory)
{
    _root_directory = root_directory;
}

std::string Request_parser::getIndex() const
{
    return _index;
}

void    Request_parser::setIndex(std::string index)
{
    _index = index;
}

std::vector<std::pair<std::string, std::string> >   Request_parser::getRequestHeaders()
{
    //std::cout << "OKAY1" << std::endl;
    return _request_headers;
}

std::string Request_parser::getTitle() const
{
    return _title;
}

void    Request_parser::setTitle(std::string title)
{
    _title = title;
}

std::size_t Request_parser::getBytesInsideBody() const
{
    return _bytes_inside_body;
}

bool    Request_parser::getIsShortBody() const
{
    return _is_short_body;
}

bool    Request_parser::getIsLongBody() const
{
    return _is_long_body;
}

int Request_parser::getFdLongBody() const
{
    return _fd_long_body;
}

FILE    *Request_parser::getFileLongBody() const
{
    return _file_long_body;
}

std::size_t Request_parser::getBytesInsideFileBody() const
{
    return _bytes_inside_file_body;
}

//
// NGINX LIT L'URI DE GAUCHE A DROITE ET CONSIDERE LE PREMIER ELEMENT EN .PHP COMME ETANT LE SCRIPT A EXECUTER
//

// SI PLUSIEURS FOIS CONTENT_LENGTH ALORS RETOURNER 404 BAD REQUEST


// A TESTER IMPORTANT
//GET /test2.php HTTP/1.1
//Host: webserv.tech
//User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:107.0) Gecko/20100101 Firefox/107.0
//Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8
//Accept-Language: fr,fr-FR;q=0.8,en-US;q=0.5,en;q=0.3
//Accept-Encoding: gzip, deflate
//Content-Type: application/x-www-form-urlencoded
//Content-Length : 60
//Origin: http://webserv.tech
//Connection: close
//Referer: http://webserv.tech/upload2.html
//Upgrade-Insecure-Requests: 1
//Content-Length: 60
