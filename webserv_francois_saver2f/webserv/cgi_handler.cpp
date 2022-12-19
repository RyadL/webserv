#include "cgi_handler.hpp"

Cgi_handler::Cgi_handler(Request_parser req_parser)
    :_req_parser(req_parser)//, _meta_variables(31)
{
    std::cout << "Cgi_handler constructor" << std::endl;
}
#include <cstdio>
void    Cgi_handler::execute_delete_request()
{
    std::cout << "EXECUTE DELETE REQUEST" << std::endl;
    if (!_req_parser.getBodyBuffer())
    {
        std::cout << "BODY" << std::endl;
        std::cout << "415 Unsupported Media Type" << std::endl;
        //HTTP/1.1 415 Unsupported Media Type
    }
    char filename[] = "/home/dimitri/Documents/42/webserv/uploads/unuploadtest.txt";
    if (!std::remove(filename))
        std::cout << "Deletion succeed" << std::endl;
    else
        std::cout << "Deletion failed" << std::endl;
}

void    Cgi_handler::execute_request()
{
    // BUG POUR LE MOMENT SI METHOD INVALIDE CAR TROU DANS LE PARSING
    if (_req_parser.getRequestMethod() == "GET")
        execute_get_request();
    else if (_req_parser.getRequestMethod() == "POST")
        execute_post_request();
    else
        execute_delete_request();
        
}
#include <cstdlib>
#include <cstring>

void    Cgi_handler::create_meta_variables(std::string var, std::string value, int *index)
{
    var += "=";
    var += value;
    _meta_env[*index] = new char[var.length() + 1];
    std::size_t    i = 0;
    for (; i != var.length(); i++)
    {
        _meta_env[*index][i] = var[i];
    }
    _meta_env[*index][i] = 0;  
    (*index)++;
}

void    Cgi_handler::add_header_meta_variables(int *index)
{
    
    for (std::vector<std::pair<std::string, std::string> >::const_iterator  it = _req_parser.getRequestHeadersPtr()->begin(); it != _req_parser.getRequestHeadersPtr()->end(); it++)
    {
        if (!it->first.empty())
            create_meta_variables("HTTP_" + it->first, it->second, index);
    }
    
}

void    Cgi_handler::add_request_line_meta_variables(int *index)
{
    create_meta_variables("REQUEST_METHOD", _req_parser.getRequestMethod(), index);
    create_meta_variables("PATH_INFO", "/home/dimitri/Téléchargements/webserv_francois/webserv", index);
    create_meta_variables("SCRIPT_NAME", "/test.php", index);
    create_meta_variables("SCRIPT_FILENAME", "/home/dimitri/Téléchargements/webserv_francois/webserv/test.php", index);
}

void    Cgi_handler::add_server_meta_variables(int *index)
{
    create_meta_variables("SERVER_PROTOCOL", "HTTP/1.1", index);
    create_meta_variables("REDIRECT_STATUS", "200", index);
    /*
    create_meta_variables("REDIRECT_STATUS", "200", index);
    create_meta_variables("CONTENT_TYPE", _req_parser.getRequestContentType(), index);
    create_meta_variables("CONTENT_LENGTH", _req_parser.getRequestContentLength(), index);
    create_meta_variables("GATEWAY_INTERFACE", "CGI/1.1", index);
    create_meta_variables("REMOTE_ADDR", "", index);
    create_meta_variables("REMOTE_HOST", "", index);
    create_meta_variables("SERVER_NAME", "", index);
    create_meta_variables("SERVER_PORT", "", index);
    create_meta_variables("SERVER_PROTOCOL", "HTTP/1.1", index);
    create_meta_variables("SERVER_PROTOCOL", "HTTP/1.1", index);
    create_meta_variables("SERVER_SOFTWARE", "webserv", index);
    */
}

void    Cgi_handler::execute_get_request()
{
    std::cout << "EXECUTE GET REQUEST" << std::endl;
    int status;
    int fd_pipe[2];
    
    
    if (pipe(fd_pipe) == -1)
        return ;
    int pid = fork();
    if (!pid)
    {

        close(fd_pipe[0]);
        std::cout << "OKAY0" << std::endl;
        int i = 0;
        _meta_env = new char*[16 + _req_parser.getRequestHeadersPtr()->size() + 1];
        /*
        add_request_line_meta_variables(&i);
        add_header_meta_variables(&i);
        */
        
        add_header_meta_variables(&i);
        add_request_line_meta_variables(&i);
        add_server_meta_variables(&i);
        char *args[] = {(char *)"/usr/bin/php-cgi", (char *)"/home/dimitri/Téléchargements/webserv_francois/webserv/test.php", NULL};
        _meta_env[i] = NULL;
        dup2(fd_pipe[1], STDOUT_FILENO);
        close(fd_pipe[1]);
        //
        close(STDIN_FILENO);
        //
        execve(args[0], args, _meta_env);
        std::cout << "EXECVE A FAIL" << std::endl;
       exit(1);
    }
    waitpid(pid, &status, 0);
    close(fd_pipe[1]);

    //char buffer[4096];
    _buffer = new char[4096];
    std::cout << "OKAY1" << std::endl;
    _length = read(fd_pipe[0], _buffer, 2000);
    _buffer[_length] = 0;
    std::cout << "LENGTH0: " << _length << std::endl;
    std::cout << "BUFFER: " << std::endl;
    std::cout << _buffer;
    std::cout << "OKAY2" << std::endl;
}

void    Cgi_handler::execute_post_request()
{
    std::cout << "EXECUTE POST REQUEST" << std::endl;
    int status;
    int pid = fork();
    if (!pid)
    {
        
        int fd_pipe[2];
        int i = 0;
        _meta_env = new char*[16 + _req_parser.getRequestHeadersPtr()->size() + 1];

        
        add_header_meta_variables(&i);
        add_request_line_meta_variables(&i);
        add_server_meta_variables(&i);
        _meta_env[i] = NULL;
        if (pipe(fd_pipe) == -1)
            return ;
        dup2(fd_pipe[0], STDIN_FILENO);
        close(fd_pipe[0]);
        int save_out = dup(1);
        dup2(fd_pipe[1], STDOUT_FILENO);
        close(fd_pipe[1]);
        for (int i = 0; i < 38657 ; i++)
            std::cout << _req_parser.getBodyBuffer()[i];
        std::cout.flush();
        char *args[] = {(char *)"/home/dimitri/Téléchargements/webserv_francois/webserv/ubuntu_cgi_tester", (char *)"/home/dimitri/Téléchargements/webserv_francois/webserv/test.php", NULL};
        dup2(save_out, STDOUT_FILENO);
        close(save_out);
        execve(args[0], args, _meta_env);
        std::cout << "FAILED" << std::endl;
        exit(1);
    }
    waitpid(pid, &status, 0);
}

char    *Cgi_handler::getBuffer() const
{
    return _buffer;
}

std::size_t Cgi_handler::getLength() const
{
    return _length;
}