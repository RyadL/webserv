#include "request_parser.hpp"
#include "cgi_handler.hpp"
#include "../response/static_response.hpp"
#include <iostream>

int main()
{
    
    const char    *buffer = "GET /test.php HTTP/1.1\r\n"
        "Host: 127.0.0.1:13856\r\n"
        "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:106.0) Gecko/20100101 Firefox/106.0\r\n"
        "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\r\n"
        "Accept-Language: fr,fr-FR;q=0.8,en-US;q=0.5,en;q=0.3\r\n"
        "Accept-Encoding: gzip, deflate\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: 4\r\n"
        "Origin: http://webserv.tech\r\n"
        "Connection: close\r\n"
        "Referer: http://webserv.tech/upload.php\r\n"
        "Upgrade-Insecure-Requests: 1\r\n"
        "\r\n"
        "TEST";




    

    //"http://webserv.tech/test.php/foo.php/bar.php/baz.php"
    //"/cgi_scripts/test.php"

    //std::cout << buffer;

    std::cout << "ICI0" << std::endl;
    std::string str;

    str = buffer;
    //std::cout << str << std::endl;
    //Static_response test;
    
    Request_parser  req_pars(buffer, 0);
    Static_response test(req_pars, 0);
    //test.setRequestParser(req_pars);
    req_pars.parse_request();
    Cgi_handler cgi_hdl(req_pars);

    cgi_hdl.execute_request();
    
    test.prepare_cgi_response(cgi_hdl.getBuffer(), cgi_hdl.getLength());
    return 0;
}

// FAIRE GAFFE AUX DIFFERENTS TYPES DE WHITESPACES
// REDIRIGER STDERROR
//Request_handler

// VOIR DANS QUELLE CLASSE STOCKER LE FILE DESCRIPTOR