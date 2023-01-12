#include "../includes/webserv.hpp"

Cgi_handler::Cgi_handler(Request_parser req_parser, Static_response& rsp_hdl)
    :_req_parser(req_parser), _rsp_hdl(&rsp_hdl), _rsp_buf_ptr(&_rsp_buf)//, _meta_variables(31)
{
    std::cout << "Cgi_handler constructor" << std::endl;
    std::cout << "_req_parser.getBytesInsideBody()0: " << _req_parser.getBytesInsideBody() << std::endl;
    //exit(0);
}

Cgi_handler::~Cgi_handler()
{
    std::cout << "Cgi_handler default destructor" << std::endl;
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
    // cgi_parsing
    std::cout << "_req_parser.getBytesInsideBody()1: " << _req_parser.getBytesInsideBody() << std::endl;
    if (_req_parser.getRequestMethod() == "GET")
        execute_get_request();
    else if (_req_parser.getRequestMethod() == "POST")
        execute_post_request();
    else if (_req_parser.getRequestMethod() == "DELETE")
        execute_delete_request();
    else
        throw CgiScriptInvalidMethod();
}
#include <cstdlib>
#include <cstring>

void    Cgi_handler::create_meta_variables(std::string var, std::string value, int *index)
{
    std::cout << "var: " << var << " et value: " << value << std::endl;
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
    create_meta_variables("QUERY_STRING", _req_parser.getRequestQueryString(), index);
    //std::cout << "QUERY_STRING: " << _req_parser.getRequestQueryString() << std::endl;
    create_meta_variables("PATH_INFO", _req_parser.getRequestPathInfo(), index);
    create_meta_variables("SCRIPT_NAME", _req_parser.getRequestScriptName(), index);
    //std::cout << "REQUEST SCRIPT_NAME: " << _req_parser.getRequestScriptName() << std::endl;
    create_meta_variables("SCRIPT_FILENAME", _req_parser.getRequestScriptFilename(), index);
    std::cout << "SCRIPT_FILENAME0: " << _req_parser.getRequestScriptFilename() << std::endl;
    //std::cout << "SCRIPT_FILENAME1: " << _req_parser.getRootDirectory() + _req_parser.getRequestScriptName() << std::endl;
    //exit(0);
}

void    Cgi_handler::add_server_meta_variables(int *index)
{
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
}

void    Cgi_handler::execute_get_request()
{
    //std::cout << "EXECUTE GET REQUEST" << std::endl;
    int status;
    struct stat sb;
    FILE*   tmpf;

    //const char* _cgi_launcher = "/usr/bin/php-cgi";
    const char* _cgi_launcher = "/usr/bin/python3";
    std::string script_filename(_req_parser.getRequestScriptFilename());

    std::cout << "SCRIPT_FILENAME: " << script_filename << std::endl;
    tmpf = std::tmpfile();
    if (!tmpf)
        throw std::exception();
    if (stat(&script_filename[0], &sb) != 0)
    {
        close(fileno(tmpf));
        throw  CgiScriptNotFound();
    }
    std::cout << "_cgi_launcher: " << _cgi_launcher << std::endl;

    int pid = fork();
    if (!pid)
    {
        int i = 0;
        try
        {
            _meta_env = new char*[16 + _req_parser.getRequestHeadersPtr()->size() + 1];
        }
        catch(const std::exception& e)
        {
            close(fileno(tmpf));
            throw ;
        }
        add_header_meta_variables(&i);
        add_request_line_meta_variables(&i);
        add_server_meta_variables(&i);
        _meta_env[i] = NULL;
        char *args[] = {(char*)_cgi_launcher, &script_filename[0], NULL};
        dup2(fileno(tmpf), STDOUT_FILENO);
        std::fclose(tmpf);
        close(STDIN_FILENO);
        execve(args[0], args, _meta_env);
        exit(1);
    }
    waitpid(pid, &status, 0);
    if (WEXITSTATUS(status))
    {
        close(fileno(tmpf));
        throw CgiScriptFailed();
    }
    std::rewind(tmpf);
    receiveCgiResponse(fileno(tmpf));
}

void    Cgi_handler::write_body_in_tmpfile(FILE *tmpf)
{
    std::cout << "_req_parser.getBodyBuffer()667: " << _req_parser.getBodyBuffer() << std::endl;
    std::cout << "_req_parser.getBytesInsideBody()667: " << _req_parser.getBytesInsideBody() << std::endl;
    
    std::size_t offset = 0;
    std::size_t bytes_count = _req_parser.getBytesInsideBody();
    int fd_pipe[2];
    std::size_t pipe_bytes_max = fcntl(fd_pipe[1], F_GETPIPE_SZ);
    std::size_t pipe_bytes = 0;
    int save_stdin = dup(STDIN_FILENO);
    std::size_t ret;
    char    buf[4096];
        
    if (pipe(fd_pipe) == -1)
        return ;
    dup2(fd_pipe[0], STDIN_FILENO);
    close(fd_pipe[0]);

    if (_req_parser.getIsShortBody())
    {
        while (bytes_count && offset < 16 * 1024)
        {
            if (bytes_count > 4096)
            {
                write(fd_pipe[1], _req_parser.getBodyBuffer(), 4096);
                bytes_count -= 4096;
            }
            else
            {
                write(fd_pipe[1], _req_parser.getBodyBuffer(), bytes_count);
                bytes_count = 0;
            }        
        }  
    }
    if (_req_parser.getIsLongBody())
    {
        int tmpf_fd = _req_parser.getFdLongBody();
        while (bytes_count)
        {
            if (pipe_bytes > pipe_bytes_max)
            {
                close(fd_pipe[1]);
                if (pipe(fd_pipe) == -1)
                    return ;
                dup2(fd_pipe[0], STDIN_FILENO);
                close(fd_pipe[0]);
                pipe_bytes = 0;
            }
            ret = read(tmpf_fd, buf, 4096);
            write(fd_pipe[1], buf, ret);
        }
        close(fd_pipe[1]);
    }
    else
        close(fd_pipe[1]);

    //!!!!!! GERER LONG BODY !!!!!! FAIRE UN DEUXIEME PIPE
    

}

void    Cgi_handler::execute_post_request()
{
    std::cout << "EXECUTE POST REQUEST" << std::endl;
    int status;
    struct stat sb;
    FILE*   tmpf;
    std::cout << "_req_parser.getBytesInsideBody()2: " << _req_parser.getBytesInsideBody() << std::endl;
   
    //const char* _cgi_launcher = "/usr/bin/php-cgi";
    const char* _cgi_launcher = "/usr/bin/python3";
    std::string script_filename(_req_parser.getRequestScriptFilename());

    tmpf = std::tmpfile();
    if (!tmpf)
        throw   std::exception();
    if (stat(&script_filename[0], &sb) != 0)
    {
        close(fileno(tmpf));
        throw CgiScriptNotFound();
    }
    std::cout << "_cgi_launcher: " << _cgi_launcher << std::endl;
    int pid = fork();
    if (!pid)
    {
        FILE* tmpf2 = std::tmpfile();
        if (!tmpf2)
        {
            close(fileno(tmpf));
            exit(1);
        }
        int i = 0;
        try
        {
            _meta_env = new char*[16 + _req_parser.getRequestHeadersPtr()->size() + 1];
        }
        catch(const std::exception& e)
        {
            close(fileno(tmpf));
            close(fileno(tmpf2));
            exit(1);
        }
        add_header_meta_variables(&i);
        add_request_line_meta_variables(&i);
        add_server_meta_variables(&i);
        _meta_env[i] = NULL;
        write_body_in_tmpfile(tmpf);
        char *args[] = {(char *)_cgi_launcher, &script_filename[0], NULL};
        dup2(fileno(tmpf), STDOUT_FILENO);
        close(fileno(tmpf));
        execve(args[0], args, _meta_env);
        exit(1);
    }
    waitpid(pid, &status, 0);
    if (WEXITSTATUS(status))
    {
        close(fileno(tmpf));
        throw CgiScriptFailed();
    }
    std::rewind(tmpf);
    receiveCgiResponse(fileno(tmpf));
}

std::size_t Cgi_handler::find_header_end(std::size_t count_bytes)
{
    std::size_t header_end = 0;
    int i = 0;
    std::size_t j;
    int ret = 1;
    char    buf[4096];

    std::cout << "_rsp_buf.getBuffersSetPtr()->size: " << _rsp_buf.getBuffersSetPtr()->size() << std::endl;
    std::cout.write((*_rsp_buf.getBuffersSetPtr())[i].first, (*_rsp_buf.getBuffersSetPtr())[i].second);
    std::cout << "(*_rsp_buf.getBuffersSetPtr())[i].second: " << (*_rsp_buf.getBuffersSetPtr())[i].second << std::endl;
    //exit(0);
    for (; header_end < count_bytes && i < 8; header_end++)
    {
        j = 0;
        for (; j < (*_rsp_buf.getBuffersSetPtr())[i].second; j++)
        {
            if (j + 4 < (*_rsp_buf.getBuffersSetPtr())[i].second && !std::memcmp((*_rsp_buf.getBuffersSetPtr())[i].first + j, "\r\n\r\n", 4))
            {
                std::cout << "HEADER_END FOUND0" << std::endl;
                //std::cout << (*_rsp_buf.getBuffersSetPtr())[i].first << std::endl;
                std::cout << "J0: " << j << std::endl;
                std::cout << "OKAY0" << std::endl;
                
                //std::cout << "EXIT667" << std::endl;
                //exit(0);
                //found = true;
                //break;
                //header_end = j;
                _rsp_buf_ptr->setHeaderEndType(2);
                header_end += j;
                //std::cout << "EXIT667" << std::endl;
                //exit(0);
                return header_end + 2;
            }
            if (j + 2 < (*_rsp_buf.getBuffersSetPtr())[i].second && !std::memcmp((*_rsp_buf.getBuffersSetPtr())[i].first + j, "\n\n", 2))
            {
                std::cout << "HEADER_END FOUND1" << std::endl;
                //std::cout << (*_rsp_buf.getBuffersSetPtr())[i].first << std::endl;
                std::cout << "J1: " << j << std::endl;
                std::cout << "OKAY1" << std::endl;
                
                //std::cout << "EXIT667" << std::endl;
                //exit(0);
                _rsp_buf_ptr->setHeaderEndType(1);
                header_end += j;
                //std::cout << "EXIT667" << std::endl;
                //exit(0);
                return header_end + 1;
            }
        }
        i++;
        header_end += j;
       
    }
    if (_rsp_buf_ptr->getIndex() == 8)
    {
        while (ret)
        {
            ret = read(fileno(_rsp_buf_ptr->getTmpFile()), buf, 4096);
            for (std::size_t    i = 0; i < ret; i++)    
            {
                if (!std::memcmp((*_rsp_buf.getBuffersSetPtr())[i].first + j, "\r\n\r\n", 4))
                {
                    header_end += j;
                    

                    return header_end + 2;
                }
                if (!std::memcmp((*_rsp_buf.getBuffersSetPtr())[i].first + j, "\n\n", 2))
                {

                    header_end += j;
                    return header_end + 1;
                }

            }
        }
    }

    std::cout << "HEADER_END667: " << header_end << std::endl; // IF HEADER_END == 0 THEN ERROR
    std::cout << "EXIT667" << std::endl;
    throw CgiScriptInvalid();
    exit(0);
    return 0;
}

void    Cgi_handler::print_cgi_response_bufs(std::size_t i, FILE* tmpf)
{
    char    test[4096];
    int ret;
    std::cout << "PRINT_CGI_RESPONSE_BUFS" << std::endl;
    for (std::size_t index = 0; index < i + 1 && index < 8; index++)
    {
        std::cout << "START" << std::endl;
        std::cout.write((*_rsp_buf.getBuffersSetPtr())[index].first, (*_rsp_buf.getBuffersSetPtr())[index].second) << "END" << std::endl;       
        std::cout << "(*_rsp_buf.getBuffersSetPtr())[index].second: " << (*_rsp_buf.getBuffersSetPtr())[index].second << std::endl;
    }
    //std::cout.write((*_rsp_buf.getBuffersSetPtr())[i].first, count_bytes) << "END" << std::endl;
    if (i > 8)
    {
        
        ret = read(fileno(tmpf), test, 4096);
        write(1, test, ret);
        std::cout << "END" << std::endl;
        std::rewind(tmpf);
    }    
}

void    Cgi_handler::receiveCgiResponse(int fd)
{
    int ret;
    std::size_t count_bytes = 0;
    std::size_t i = 0;
    std::size_t j = 0;
    FILE    *tmpf;
    struct stat sb;
    char    buf[4096];

    if (fstat(fd, &sb) == -1)
    {
        std::cerr << "ERROR" << std::endl;
        throw std::exception();
        return ;
    }
    if (!sb.st_size)
    {
        throw CgiScriptFailed();
        return ;
    }
    while (i < 8)
    {
        try
        {
            _rsp_buf.add_buffer();
        }
        catch(const std::exception& e)
        {
            throw std::exception();
        }
        ret = read(fd, (*_rsp_buf.getBuffersSetPtr())[i].first, 4096);
        if (!ret)
        {
            _rsp_buf.getBuffersSetPtr()->pop_back();
            break ;
        }  
        count_bytes += ret;
        (*_rsp_buf.getBuffersSetPtr())[i].second = ret;
        if (ret == 4096)
            i++;
    }

    if (sb.st_size > 4096 * 8)
    {
        tmpf = std::tmpfile();
        if (!tmpf)
            throw std::exception();
        while (ret)
        {
            ret = read(fd, buf, 4096);
            if (ret == -1)
            {
                close(fileno(tmpf));
                throw std::exception();
            }
            count_bytes += ret;
            write(fileno(tmpf), buf, ret);
        }
        std::rewind(tmpf);
        i++; 
    }
    _rsp_buf.setIndex(i);
    _rsp_buf_ptr->setCountBytes(count_bytes);
    _rsp_buf_ptr->setTmpFile(tmpf);
    _rsp_buf_ptr->setHeaderEnd(find_header_end(count_bytes));
}

std::size_t Cgi_handler::getLength() const
{
    return _length;
}

Static_response *Cgi_handler::getResponseHandler() const
{
    return _rsp_hdl;
}

std::vector<std::string>    Cgi_handler::getHeaders() const
{
    return _headers;
}

Response_buffers    *Cgi_handler::getResponseBuffersPtr() const
{
    return _rsp_buf_ptr;
}