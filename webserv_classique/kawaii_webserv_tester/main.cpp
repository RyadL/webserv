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

# include "httpKawaiiRequest.hpp"

#define IP_ADDRESS  "127.0.0.1"
#define SERV_PORT 13878

void print_number(int* myInt) {
  assert (myInt!=NULL);
  printf ("%d\n",*myInt);
}

int main (int argc, char *argv[])
{

    /*
    if (argc != 2)
    {
        std::cout << "./kawaii_tester http://<HOST>:<PORT>" << std::endl;
        return 1;
    }
    */
    /*
    int client_socket_fd;
    struct sockaddr_in servaddr;




    client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket_fd == -1)
    {
        std::cerr << "Error: socket failed" << std::endl;
        return 1;
    }
    std::memset(&servaddr, 0, sizeof(sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    if (inet_pton(AF_INET, IP_ADDRESS, &servaddr.sin_addr) == -1 && errno == EAFNOSUPPORT)
    {
        std::cerr << "Error: inet_pton failed" << std::endl;
        return 1;
    }
    if (connect(client_socket_fd, (struct sockaddr*) &servaddr, sizeof(sockaddr_in)) == -1)
    {
        std::cerr << "Error: connect failed" << std::endl;
        return 1;
    }
    */
    HttpKawaiiRequest   kawaii_req;

    try
    {
        kawaii_req.kawaiiConnection();
    }
    catch(const HttpKawaiiRequest::ConnectionFailure& e)
    {
        std::cerr << "Connection failed" << std::endl;;
        return 1;
    }

    kawaii_req.craftRawRequest("POST /upload.php HTTP/1.1 Host: webserv.tech User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:108.0) Gecko/20100101 Firefox/108.0 Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8 Accept-Language: fr,fr-FR;q=0.8,en-US;q=0.5,en;q=0.3 Accept-Encoding: gzip, deflate Content-Type: multipart/form-data; boundary=---------------------------49956688218506704373806053753 Content-Length: 369 Origin: http://webserv.tech Connection: close Referer: http://webserv.tech/upload.html Upgrade-Insecure-Requests: 1 -----------------------------49956688218506704373806053753 Content-Disposition: form-data; name=\"fileToUpload\"; filename=\"example.txt\" Content-Type: text/plain Status: 404 Not Found\r\n Content-type: text/html; charset=UTF-8\r\n\r\nNo input file specified. -----------------------------49956688218506704373806053753 Content-Disposition: form-data; name=\"fileToUpload2\"; filename="" Content-Type: application/octet-stream -----------------------------49956688218506704373806053753 Content-Disposition: form-data; name=\"submit\" Upload Image -----------------------------49956688218506704373806053753--\r\n\r\n");
    kawaii_req.sendRawRequest();



    /*
    kawaii_req.craftRawRequest("GET / HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\nCECI N'EST PAS UN TEST");

    kawaii_req.sendRawRequest();
    */

    
    //kawaii_req.getRequest("/test");

    
    //kawaii_req.addBody("CECI EST UN TEST");
    
    //kawaii_req.addHeader("Connection: close");

   // std::string str;


    /*
    kawaii_req.craftRawRequest("GET / HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\nCECI N'EST PAS UN TEST");

    kawaii_req.sendRawRequest();
    */

    
    //kawaii_req.getRequest("/test");

    
    //kawaii_req.addBody("CECI EST UN TEST");
    
    //kawaii_req.addHeader("Connection: close");

   // std::string str;

   
    
    

    return 0;
}