
#include "static_response.hpp"
#include <limits>

// SERVE STATIC CONTENT

int main()
{
    std::ifstream   file;

    //file.open("/home/dimitri/Téléchargements/webserv_francois/response/file.txt");
    file.open("/home/dimitri/Téléchargements/webserv_francois/response/index.html");
    /*
    setStatusCode("200");
    std::cout << "STATUS CODE: " << getStatusCode() << std::endl;

    setReasonPhrase("OK");
    std::cout << "REASON PHRASE: " << getReasonPhrase() << std::endl;
    */
    if (!file.is_open())
        std::cout << "FAILURE" << std::endl;
    file.ignore(std::numeric_limits<std::streamsize>::max());
    std::streamsize _length = file.gcount();
    //std::cout << "LENGTH: " << _length << std::endl;
    file.clear();   //  Since ignore will have set eof.
    file.seekg( 0, std::ios_base::beg );

    std::string response_line("HTTP/1.1");
    response_line += " ";
    response_line += "200";
    response_line += " ";
    response_line += "OK";
    response_line += "\r\n";

    std::string content_length("CONTENT_LENGTH: ");
    std::ostringstream ss;
    ss << _length;
    content_length += ss.str();
    content_length += "\r\n";
    //std::cout << "CONTENT_LENGTH: " << content_length << std::endl;
    char *_buffer = new char[2000];
    //_buffer[length + 1];
    std::cout << "OKAY0" << std::endl;
    std::memcpy(_buffer, response_line.c_str(), response_line.length());
    _buffer[response_line.length()] = 0;
    std::cout << "STRLEN0: " << std::strlen(_buffer) << std::endl;
    std::cout.write(_buffer, std::strlen(_buffer));

    std::cout << "OKAY1" << std::endl;
    std::memcpy(_buffer + response_line.length(), content_length.c_str(), content_length.length());
    _buffer[response_line.length() + content_length.length()] = 0;

    std::cout << "STRLEN1: " << std::strlen(_buffer) << std::endl;
    std::cout.write(_buffer, std::strlen(_buffer));

    

    file.read(_buffer + response_line.length() + content_length.length(), _length);

    std::cout << "OKAY2" << std::endl;
    //std::memcpy(_buffer + response_line.length() + content_length.length(), content_length.c_str(), content_length.length());
    _buffer[response_line.length() + content_length.length() + _length] = 0;

    std::cout << "STRLEN: " << std::strlen(_buffer) << std::endl;
    std::cout.write(_buffer, std::strlen(_buffer));
    //std::memcpy(_buffer + response_line.length(), content_length.c_str(), content_length.length());
    //file.read(_buffer, _length);
    
    //_buffer[response_line.length() + content_length.length()] = 0;
    //std::cout << "BUFFER: " << _buffer << std::endl; // PRINTER DANS LE CLIENT
    //std::cout << "BUFFER: " << std::endl;

    //std::cout.write(buffer, length);
    //dup2(_fd, STDIN_FILENO);
    //std::cout.write(buffer, length);
    
    return 0;
}

/*
int main()
{
    //char buffer[4096];
    char    *buffer1;
    int size = 1;
    int i = 0;
    buffer1 = new char[size];
    Http_response   test;

    test.setStatusCode("200");
    std::cout << "STATUS CODE: " << test.getStatusCode() << std::endl;

    test.setReasonPhrase("OK");
    std::cout << "REASON PHRASE: " << test.getReasonPhrase() << std::endl;

    
    
    std::ifstream    file("file.txt");
    
    std::cout << "OKAY0" << std::endl;
    char * buffer2;
    int j;
    int buf_size = 1;
    while (file)
    {
        file.read(buffer1 + i, 1);
        if (size == buf_size)
        {
            size *= 2;
            buffer2 = new char[size];
            
            int j = 0;
            for (; j < i; j++)
            {

                buffer2[j] = buffer1[j];
            }
            buffer2[j] = buffer1[j];   

            delete [] buffer1;
            buffer1 = buffer2;
        }

        buf_size++;
        i++;
        
    }

    buffer1[--i] = 0;
    std::cout << buffer1 << std::endl;
    std::cout << "OKAY1" << std::endl;
    delete [] buffer1;
    return 0;
}
*/

/*
int main()
{
    //char buffer[4096];
    char    *buffer1;
    int size = 1;
    int i = 0;
    buffer1 = new char[size];
    Http_response   test;

    test.setStatusCode("200");
    std::cout << "STATUS CODE: " << test.getStatusCode() << std::endl;

    test.setReasonPhrase("OK");
    std::cout << "REASON PHRASE: " << test.getReasonPhrase() << std::endl;

    
    
    std::ifstream    file("file.txt");
    
    std::cout << "OKAY0" << std::endl;
    char * buffer2;
    int j;
    while (file)
    {
        file.read(buffer1 + i, 1);
        //std::cout << file.gcount() << std::endl;
        if (file.gcount() == 1)
        {
            
            buffer2 = new char[size++ + 1];
            int j = 0;
            for (; j < i; j++)
            {

                buffer2[j] = buffer1[j];
            }
            buffer2[j] = buffer1[j];   

            delete [] buffer1;
            buffer1 = buffer2;
        }

        i++;

    }

    buffer1[--i] = 0;
    std::cout << buffer1 << std::endl;
    std::cout << "OKAY1" << std::endl;
    delete [] buffer1;
    return 0;
}
*/

/*
file.ignore( std::numeric_limits<std::streamsize>::max() );
std::streamsize length = file.gcount();
file.clear();   //  Since ignore will have set eof.
file.seekg( 0, std::ios_base::beg );
*/
/*
#include "http_response.hpp"

#include <limits>

int main()
{
    //char buffer[4096];
    char    *buffer1;
    int size = 2;
    int i = 0;
    buffer1 = new char[size];
    Http_response   test;

    test.setStatusCode("200");
    std::cout << "STATUS CODE: " << test.getStatusCode() << std::endl;

    test.setReasonPhrase("OK");
    std::cout << "REASON PHRASE: " << test.getReasonPhrase() << std::endl;

    std::ifstream   file;

    file.open("file.txt");

    file.ignore( std::numeric_limits<std::streamsize>::max() );
    std::streamsize length = file.gcount();
    std::cout << "LENGTH: " << length << std::endl;
    file.clear();   //  Since ignore will have set eof.
    file.seekg( 0, std::ios_base::beg );
    return 0;
}
*/