#ifndef SOCKET_CLASS_HPP
#define SOCKET_CLASS_HPP

#include <iostream>
#include "client_buffer.hpp"
#include <errno.h>
#include <time.h>

class socket_class 
{
    public :

    socket_class()
    {
        //std::cout << "socket_class constructor" << std::endl;
    }

    ~socket_class()
    {
        //std::cout << "socket_class destructor" << std::endl;
    }

    //////////////////////////////// MAP OPERATION ///////////////////////////////

    void add_client(int socket_client)
    {
        //std::cout << "Le CLIENT numero " << socket_client << " est ajoute" << std::endl << std::endl;
        if ((tree.insert(std::pair<int, client_buffer*>(socket_client, new client_buffer()))).second == false)
        {
            //std::cout << "CLIENT ALREADY EXIST IN TREE " << std::endl;
            if (!tree.insert(std::pair<int, client_buffer*>(socket_client, new client_buffer())).first->second->state_suspended)
            {
                //std::cout << "CLIENT IS NOT SUSPENDED SO HE IS SUPPRESSED " << std::endl << std::endl;
                tree.erase(tree.insert(std::pair<int, client_buffer*>(socket_client, new client_buffer())).first);
                tree.insert(std::pair<int, client_buffer*>(socket_client, new client_buffer()));
            }
            else
            {
                //std::cout << "CLIENT WERE SUSPENDED" << std::endl << std::endl;
                std::map<int, client_buffer*>::iterator it = tree.find(socket_client);
            }
        }
    }

    int traitement(int file_descriptor)
    {
        std::map<int, client_buffer*>::iterator it = tree.find(file_descriptor);

        time(&it->second->timer);
        it->second->secondsy2k1 = difftime(it->second->timer,mktime(&it->second->y2k1));

        it->second->last_EPOLL2 = 0;
        it->second->last_EPOLL = 0;

        //std::cout << "LOOK (￢‿￢ ) ---> " << it->second->state_suspended << std::endl << std::endl;

        int    (socket_class::*f[5])( std::map<int, client_buffer*>::iterator ) = { &socket_class::get_header, &socket_class::drive_body, &socket_class::get_short_body, &socket_class::get_long_body, &socket_class::get_chunk_body } ;
        
        for (int i = 0; i < 5; i++)
        {
            if (it->second->currently[i] == true)
                if (!(this->*f[i])(it))
                    return 0;
        }

        return 1;
        //std::cout << "Fin de traitement" << std::endl << std::endl;
    }

    ////////////////////////////// BUFFER OPERATION ////////////////////////////////////


    int is_there_a_body(char* buffer, int size)
    {
        for ( int i = 0; i < size; i++)
        {
            if (!strncmp( buffer + i, "Content-Length", 14 ))
                return atoi( buffer + i + 15 );             
        }
        return 0;
    }

    bool is_there_a_chunk(char* buffer, int size)
    {
        for ( int i = 0; i < size; i++)
        {
            if (!strncmp( buffer + i, "Transfer-Encoding: chunked", 26 ))
                return true;             
        }
        return false;
    }
    
    int		ft_iswhitespace(char const c)
    {
    	if (c == ' ' || c == '\n' || c == '\t' || c == '\v'
    		|| c == '\r' || c == '\f')
    		return (1);
    	return (0);
    }
    char    to_lower(char c)
    {
        if (c >= 'A' && c <= 'Z')
            return (c + ('a' - 'A'));
        return (c);
    }
    
    int     get_digit(char c, int digits_in_base)
    {
        int max_digit;
        if (digits_in_base <= 10)
            max_digit = digits_in_base + '0';
        else
            max_digit = digits_in_base - 10 + 'a';
    
        if (c >= '0' && c <= '9' && c <= max_digit)
            return (c - '0');
        else if (c >= 'a' && c <= 'f' && c <= max_digit)
            return (10 + c - 'a');
        else
            return (-1);
    }
    
    int     ft_atoi_base(const char *str, int str_base)
    {
        int result = 0;
        int sign = 1;
        int digit;
    
        if (*str == '-')
        {
            sign = -1;
            ++str;
        }
    
        while ((digit = get_digit(to_lower(*str), str_base)) >= 0)
        {
            result = result * str_base;
            result = result + (digit * sign);
            ++str;
        }
        return (result);
    }


    //************************************************************************************************************************************************************
    //************************************************APRES TRAITEMENT FAIRE ATTENTTION AU STATE SUSPENDED *************************************************************//
    //*******************************************************************************************************************************************************************


    int get_header ( std::map<int, client_buffer*>::iterator it )
    {
       
        int ret = 0;
        int end;
        int i = 0;

        //usleep(100000);
        //memset (it->second->buffer, 0, 1024);

        memset (it->second->buffer, 0, 1025);

        ret = recv(it->first, it->second->buffer, 1024, 0);
        std::cout << "\x1B[37m*STD::COUT*Ligne 163 = it->second->buffer ->\033[0m" << it->second->buffer << "\x1B[37m<- END\033[0m" << std::endl;
        //std::cout.write(it->second->buffer,ret) << "END" << std::endl;
        if (ret == -1)
        {
            std::cout << "ret = - 1" << std::endl;
            it->second->state_suspended = true;
            return 1;
        }
        if (ret == 0)
        {
            std::cout << "CLOSE CONNECTION" << std::endl;
            it->second->state_suspended = true;
            //free_node(it);
            //tree.erase(it);
            return 0;
        }

        for (i = 0; i < ret && it->second->bytes_inside_short_header < 1024 && it->second->is_long_header == - 1; i++)
        {
            it->second->short_header[it->second->bytes_inside_short_header] = it->second->buffer[i];
            it->second->bytes_inside_short_header++;

            if (it->second->bytes_inside_short_header > 3)
            {
                if ( !strncmp( it->second->short_header + it->second->bytes_inside_short_header - 4, "\r\n\r\n", 4 ) )
                {
                    std::cout << "END WAS FOUND ヽ(o＾▽＾o)ノ" << std::endl;
                    it->second->is_short_header = true;
                    it->second->state_suspended = false;
                    it->second->currently[0] = false;
                    it->second->currently[1] = true;
                    i++;
                    //std::cout << "\033[0;31mi->\033[0m" << i << "\033[0;31m<- END\033[0m" << std::endl;
                    //std::cout << "\033[0;31mret->\033[0m" << ret << "\033[0;31m<- END\033[0m" << std::endl;
                    if (i < ret)
                    {
                        for (int j = 0; i < ret; i++, j++)
                        {
                            it->second->rest_header[j] = it->second->buffer[i];
                            it->second->bytes_inside_rest_header++;
                        }
                    }
                    //std::cout << "\033[0;31mit->second->short_header get header ->\033[0m" << it->second->short_header << "\033[0;31m<- END\033[0m" << std::endl;
                    //std::cout << "\033[0;31mit->second->rest_header get header ->\033[0m" << it->second->rest_header << "\033[0;31m<- END\033[0m" << std::endl;
                    return 1;
                }
            }
        }

        if (it->second->is_long_header == -1 && it->second->bytes_inside_short_header == 1024)
        {
            it->second->is_long_header = false;
            it->second->long_header = new char*[4]();
            for (int i = 0; i < 4; i++)
                it->second->long_header[i] = NULL;
            it->second->long_header[0] = new char[8 * 1024]();

            for (int i = 0; i < 1024; i++)
                it->second->long_header[0][i] = it->second->short_header[i];

            it->second->bytes_inside_long_header[0] = it->second->bytes_inside_short_header;

            while (i < ret)
            {
                it->second->long_header[0][i] = it->second->buffer[i];
                it->second->bytes_inside_long_header[0]++;
            }

            it->second->is_long_header = 0;
            it->second->state_suspended = true;

            return (1);
        }

        if (it->second->is_long_header > -1)
        {
            for (i = 0; i < ret; i++)
            {
                if (it->second->bytes_inside_long_header[it->second->is_long_header] > 8 * 1024)
                {
                    it->second->is_long_header++;
                    if(it->second->is_long_header == 4)
                    {
                        it->second->state_suspended = false;
                        exit(0);
                    }
                    it->second->long_header[it->second->is_long_header] = new char[8 * 1024 + 1]();
                }

                it->second->long_header[it->second->is_long_header][it->second->bytes_inside_long_header[it->second->is_long_header]] = it->second->buffer[i];
                (it->second->bytes_inside_long_header[it->second->is_long_header])++;
                
                if (it->second->bytes_inside_long_header[it->second->is_long_header] > 2)
                {
                    if ( !strncmp( it->second->long_header[it->second->is_long_header] + it->second->bytes_inside_long_header[it->second->is_long_header] - 4, "\r\n\r\n", 4 ) )
                    {
                        //std::cout << "END WAS FOUND ヽ(o＾▽＾o)ノ -> inside long header" << std::endl;
                        it->second->state_suspended = false;
                        it->second->is_short_header = false;
                        it->second->currently[0] = false;
                        it->second->currently[1] = true;
                        i++;
                        if (i < ret)
                            for (int j = 0; i < ret; i++, j++, it->second->bytes_inside_rest_header++)
                                it->second->rest_header[j] = it->second->buffer[i];
                        return 1;
                    }
                }
            }
        }

        //std::cout << "END WAS NOT FOUND (︶︹︺) " << std::endl;
        it->second->state_suspended = true;
        return 1;
    }


    int drive_body ( std::map<int, client_buffer*>::iterator it )
    {
        //std::cout << "Drive Body" << std::endl;

        if (it->second->is_short_header)
        {
            it->second->body_size = is_there_a_body(it->second->short_header, it->second->bytes_inside_short_header);
            if (it->second->body_size < 16 * 1024 && it->second->body_size)
                it->second->currently[2] = true;
            if (it->second->body_size >= 16 * 1024)
                it->second->currently[3] = true;
            if (is_there_a_chunk(it->second->short_header, it->second->bytes_inside_short_header))
            {
                it->second->is_chunk = true;
                it->second->currently[4] = true;
            }
            it->second->currently[1] = false;
            return 1;
        }
        if (it->second->is_long_header > -1)
        {
            for (int i = 0; it->second->long_header[i]; i++)
                it->second->body_size = it->second->body_size + is_there_a_body(it->second->long_header[i], it->second->bytes_inside_long_header[i]);
            
            if (it->second->body_size < 16 * 1024 && it->second->body_size)
                it->second->currently[2] = true;
            
            if (it->second->body_size >= 16 * 1024)
                it->second->currently[3] = true;
            
            for (int i = 0; it->second->long_header[i]; i++)
                if (is_there_a_chunk(it->second->long_header[i], it->second->bytes_inside_long_header[i]))
                    it->second->currently[4] = true;

            it->second->currently[1] = false;
            return 1;
        }
        return 1;
    }

    int get_chunk_body ( std::map<int, client_buffer*>::iterator it )
    {

        it->second->is_chunk = true;
        int ret = 0;
        int i = 0;
        int number = 0;

            ///////////////////////////////////////////////////REST DU HEADER//////////////////////////////////////////////

            if (!it->second->state_suspended)
            {
                //std::cout << "On entre dans le chunk avec du rest header" << std::endl;
                //std::cout << "Bytes inside chunk body = " << it->second->bytes_inside_chunk_body << " total = " << it->second->total << " to write = " << "reste header " << it->second->bytes_inside_rest_header << std::endl;
                int test = 0;
                it->second->am_i_inside_chunk = false;
                it->second->bytes_inside_chunk_body = 0;

                it->second->short_body = new char[16 * 1024 + 1]();
                memset(it->second->short_body, 0, 16 * 1024);

                if (it->second->bytes_inside_rest_header)
                {
                    //std::cout << "Il reste du header, on copy dans le short body" << std::endl;
                    //std::cout << "Bytes inside chunk body = " << it->second->bytes_inside_chunk_body << " total = " << it->second->total << " to write = " << it->second->chunktowrite << std::endl;

                    //std::cout << "Avant de chercher number regardons rest header : " << it->second->rest_header << std::endl;;

                    for (int i = 0; ; i++ )
                    {
                        if (i > 1)
                        {
                        if (!strncmp(it->second->rest_header + i - 2, "\r\n", 2))
                        {
                            //std::cout << "TROUVER" << it->second->rest_header << std::endl;;
                            it->second->chunktowrite = number = ft_atoi_base(it->second->rest_header, 16);
                            test = i;
                            break;
                        }
                        }
                    }
                    //std::cout << "voyons number  " << it->second->chunktowrite << std::endl;
                    if (number == 0)
                    {
                        //std::cout << "rest number = 0" << std::endl;
                        it->second->state_suspended = false;
                        return 1;
                    }
                    ///////////////////////////////////////////////////
                    for (int i = test; i < it->second->bytes_inside_rest_header; i++)
                    {
                            it->second->short_body[it->second->bytes_inside_chunk_body] = it->second->rest_header[i];
                            it->second->bytes_inside_chunk_body++;
                            it->second->total++;
                            it->second->chunktowrite--;
                    }
                    it->second->copy = false;
                }

                //std::cout << "Fin du reste" << std::endl;
                //std::cout << "Bytes inside chunk body = " << it->second->bytes_inside_chunk_body << " total = " << it->second->total << " to write = " << it->second->chunktowrite << std::endl;
                delete [] it->second->buffer;
                it->second->buffer = new char[16 * 1024 + 1]();
                it->second->is_long_body = false;
                it->second->is_short_body = true;
                it->second->state_suspended = true;
                if (it->second->chunktowrite)
                    it->second->am_i_inside_chunk = true;
                return (1);
            }

        ///////////////////////////////////////////////////LECTURE//////////////////////////////////////////////

        memset( it->second->buffer, 0, 16 * 1024);
        number = 0;
        ret = recv(it->first, it->second->buffer, 16 * 1024 , 0);

        //std::cout << "\x1B[37mbuffer chunk ->\033[0m" << it->second->buffer << "\x1B[37m<- END\033[0m" << std::endl;

        if (ret > 4)
        {
            if (!strncmp(it->second->buffer - 4, "\r\n\r\n", 4))
            {
                it->second->state_suspended = false;
                return 1;

            }
        }



        /*it->second->tester += it->second->buffer;

        std::size_t found = it->second->tester.find("0\r\n\r\n", it->second->tester.size() - 5);

        if (found != std::string::npos)
        {
            std::cout << "first 'needle' found at: " << found << '\n';
            it->second->state_suspended = false;
        }*/


        ///////////////////////////////////////////////////ECRITURE//////////////////////////////////////////////

        for (int i = 0; i < ret; i++)
        {

            
            ///////////////////////////////////////////////////DANS LE FD//////////////////////////////////////////////

            if (it->second->bytes_inside_chunk_body == 1024 * 16)
            {
                //std::cout << "Le short body est plein on passe tout dans le fd" << std::endl;
                it->second->is_short_body = false;
                it->second->is_long_body = true;
                //std::cout << "it->second->bytes_inside_fd_body" << it->second->bytes_inside_fd_body << std::endl;
                if (!it->second->bytes_inside_fd_body)
                {
                    //std::cout << "On cree le fd body " << it->second->bytes_inside_chunk_body << std::endl;
                    it->second->file.open(std::string("temporary").c_str(), std::ios::out|std::ios::app);
                }
                
                    for (int i = 0; i < it->second->bytes_inside_chunk_body - 10; i++)
                    {
                        it->second->file << it->second->short_body[i];
                        it->second->bytes_inside_fd_body++;
                    }

                    //std::cout << "Le file a ete remplis " << it->second->bytes_inside_chunk_body << std::endl;

                    for (int i = 0; i < 10; i++)
                    {
                        it->second->short_body[i] = it->second->short_body[it->second->bytes_inside_chunk_body - 10 + i];
                    }
                    //std::cout << "On recupere les 10 dernier characteres pour les metre en debut de short body " << it->second->bytes_inside_chunk_body << std::endl;
                    it->second->bytes_inside_chunk_body = 10;
            }

            ///////////////////////////////////////////////////DANS LE BODY//////////////////////////////////////////////

            it->second->short_body[it->second->bytes_inside_chunk_body] = it->second->buffer[i];
            it->second->bytes_inside_chunk_body++;
            it->second->total++;
            it->second->chunktowrite--;
            //std::cout << "Bytes inside chunk body = "<< "\x1B[37m" << it->second->bytes_inside_chunk_body << "\033[0m" << "total = "<< "\x1B[37m" << it->second->total << "\033[0m" << std::endl;
            //std::cout << "Bytes inside chunk body = " << it->second->bytes_inside_chunk_body << " total = " << it->second->total << "to write = " << it->second->chunktowrite << std::endl;

            if (it->second->bytes_inside_chunk_body > 1)
            {
                if (!strncmp(it->second->short_body + it->second->bytes_inside_chunk_body - 2, "\r\n", 2))
                { 
                    if ( it->second->chunktowrite < 0 && it->second->copy == false )
                    {
                        //std::cout << "Dernier separaateur dun chunk voyons voir" << it->second->bytes_inside_chunk_body << std::endl;
                        //std::cout << "Bytes inside chunk body = " << it->second->bytes_inside_chunk_body << " total = " << it->second->total << " to write = "  << it->second->chunktowrite << std::endl;
                        
                        /*for(int i = 0; i <= it->second->bytes_inside_chunk_body; i++)
                            std::cout << "inside chunk->" << (int)it->second->short_body[i] << "<-" << std::endl;

                        
                        for(int y = i - 10 ; y < i + 10; y++)
                            std::cout << "future->" << (int)it->second->buffer[y] << "<-" << std::endl;*/
                        
                        it->second->am_i_inside_chunk = false;
                        it->second->utils = it->second->bytes_inside_chunk_body;
                        it->second->copy = true;
                        continue;
                    }
                    if ( it->second->copy == true )
                    {
                        number = ft_atoi_base(it->second->short_body + it->second->utils, 16);
                        if (number == 0)
                        {
                            memset(it->second->short_body + it->second->bytes_inside_chunk_body - 3, 0, 3);
                            if (it->second->is_long_body)
                            {
                                for (int i = 0; i < it->second->bytes_inside_chunk_body - 3; i++)
                                {
                                    it->second->file << it->second->short_body[i];
                                    it->second->bytes_inside_fd_body++;
                                }
                            }
                            it->second->state_suspended = false;
                            return 1;
                        }
                        it->second->chunktowrite = number;
                        memset(it->second->short_body + it->second->utils, 0, it->second->bytes_inside_chunk_body - it->second->utils);
                        it->second->total = it->second->total - (it->second->bytes_inside_chunk_body - it->second->utils);
                        it->second->bytes_inside_chunk_body = it->second->bytes_inside_chunk_body - (it->second->bytes_inside_chunk_body - it->second->utils);
                        it->second->am_i_inside_chunk = false;
                        it->second->copy = false;
                    }
                }
            }
        }
        return (1);
        
    }

    int get_short_body ( std::map<int, client_buffer*>::iterator it )
    {

        //std::cout << "Get Short" << std::endl;
        int ret = 0;
        int i = 0;
        
        if (!it->second->state_suspended)
        {
            it->second->short_body = new char[16 * 1024 + 1]();

            for (int i = 0; i < it->second->bytes_inside_rest_header; i++)
            {
                it->second->short_body[i] = it->second->rest_header[i];
                it->second->bytes_inside_body++;

            }

            delete [] it->second->buffer;
            it->second->buffer = new char[16 * 1024 + 1]();

            it->second->state_suspended = true;
            return 1;
        }

        std::cout << "it->second->bytes_inside_body = " << it->second->bytes_inside_body << std::endl;

        //std::cout << "\x1B[37mit->second->buffer ->\033[0m" << it->second->short_body << "\x1B[37m<- END\033[0m" << std::endl;


        
        ret = recv(it->first, it->second->buffer, 16 * 1024, 0);
        
        //std::cout << "Ret short body = " << ret << std::endl;

        for (i = 0; i < ret; i++, it->second->bytes_inside_body++)
        {
            it->second->short_body[it->second->bytes_inside_body] = it->second->buffer[i];
        }

        if (it->second->bytes_inside_body == it->second->body_size)
        {
                //std::cout << "it->second->bytes_inside_body = " << it->second->bytes_inside_body << std::endl;
                //std::cout << "it->second->body_size = " << it->second->body_size << std::endl;
                it->second->is_short_body = true;
                it->second->state_suspended = false;
                it->second->currently[2] = false;
                return 1;
        }
        it->second->state_suspended = true;
        return 1;
    }


    int get_long_body ( std::map<int, client_buffer*>::iterator it )
    {
        int ret = 0;
        if (!it->second->state_suspended)
        {
            it->second->file.open(std::string("temporary").c_str(), std::ios::out | std::ios::app);
            it->second->file << it->second->rest_header;
            delete [] it->second->buffer;
            it->second->buffer = new char[5 * 1024 + 1]();
            
        }
        memset ( it->second->buffer, 0, 5 * 1024 + 1 );
        
        ret = recv(it->first, it->second->buffer, 5 * 1024, 0);
    
        it->second->file << it->second->buffer;

        if (it->second->file.tellg() == it->second->body_size)
        {
            it->second->state_suspended = false;
            it->second->is_long_body = true;
            it->second->currently[3] = false;
            return 1;
        }

        it->second->state_suspended = true; 
        return 1;
    }


    bool print_client ( int file_descriptor )
    {
        //std::cout << "Print client" << std::endl;
        std::map<int, client_buffer*>::iterator it = tree.find(file_descriptor);
        
        if (!it->second->state_suspended)
        {
            if (it->second->is_short_header)
            {
                std::cout << "1" << std::endl;
                std::cout << "\x1B[37m" << it->second->short_header << "\033[0m" << std::endl;
            }
            if (it->second->is_long_header > -1)
                for (int i = 0; it->second->long_header[i]; i++)
                {
                    std::cout << "2" << std::endl;
                    std::cout << "\x1B[37m" << it->second->long_header[i] << "\033[0m" << std::endl;
                }

            if (it->second->is_chunk)
            {
                if (it->second->is_short_body)
                {
                    std::cout << "3" << std::endl;
                    std::cout << "\x1B[37m" << it->second->short_body << "\033[0m" << std::endl;
                }
            }
            if (!it->second->is_chunk)
            {
                if (it->second->is_short_body)
                {
                    std::cout << "3" << std::endl;
                    std::cout << "\x1B[37m" << it->second->short_body << "\033[0m" << std::endl;
                }
            }
        }
        return it->second->state_suspended;
    }

    void refresh_client(int fd)
    {
        std::map<int, client_buffer*>::iterator it = tree.find(fd);

        //memset (it->second->buffer, 0, strlen(it->second->buffer));
        /*
        if (it->second->is_long_header > -1)
        {
            for (int i = 0; it->second->long_header[i]; i++)
                free(it->second->long_header[i]);
        }
        if (it->second->is_short_body)
            free(it->second->short_body);*/

        if (it->second->is_short_body)
        {
            delete [] (it->second->short_body);
            it->second->short_body = NULL;
        }

        if (it->second->is_chunk)
        {
            delete [] (it->second->short_body);
            it->second->short_body = NULL;
        }

        it->second->bytes_inside_short_header = 0;
        for (int i = 0; i < 4; i++)
            it->second->bytes_inside_long_header[i] = 0;
        it->second->bytes_inside_rest_header = 0;



        it->second->total = 0;
        it->second->bytes_inside_body = 0;
        it->second->bytes_inside_chunk_body = 0;
        it->second->chunktowrite = 0;
        it->second->body_size = 0;
        it->second->state_suspended = false;
        it->second->copy = true;
        it->second->utils = 0;

        it->second->bytes_inside_fd_body = 0;

        it->second->is_short_body = false;
        it->second->is_short_header = false;
        it->second->is_long_header = -1;
        it->second->is_long_body = false;
        it->second->currently[0] = true;
        for (int i = 1; i < 5; i++)
            it->second->currently[i] = false;

        memset (it->second->short_header, 0, 1024);
        remove("temporary");
    }

    void free_node(std::map<int, client_buffer*>::iterator it)
    {
        std::cout << "Free Node" << std::endl;

        remove("temporary");

        delete[] (it->second->buffer);
        delete[] (it->second->short_header);

        delete(it->second);
    }

    void send_data ( int file_descriptor, int number_server )
    {
        std::cout << "Send" << std::endl;
        /*std::map<int, client_buffer*>::iterator it = tree.find(file_descriptor);
        int ret = 0;
        if (number_server == 0)
        {
            ret = send(file_descriptor, "Hello world from server 1", 25, 0);
            refresh_client(it);
        }
        if (number_server == 1)
        {
            ret = send(file_descriptor, "Hello world from server 1", 25, 0);
            refresh_client(it);
    
        }
        if (number_server == 2)
        {
            ret = send(file_descriptor, "Hello world from server 1", 25, 0);
            refresh_client(it);    
        }*/
    }

    int check_time ()
    {
        struct tm y2k2 = {0};
        double seconds;

        for (std::map<int, client_buffer*>::iterator it = tree.begin(); it != tree.end(); it++) 
        {
            y2k2.tm_hour = 0;   y2k2.tm_min = 0; y2k2.tm_sec = 0;
            y2k2.tm_year = 100; y2k2.tm_mon = 0; y2k2.tm_mday = 1;

            time(&it->second->timer);  /* get current time; same as: timer = time(NULL)  */

            seconds = difftime(it->second->timer,mktime(&y2k2));

            //std::cout << "CLIENT = " << seconds - it->second->secondsy2k1 << std::endl;

            
                it->second->last_EPOLL = seconds - it->second->secondsy2k1;

                if (it->second->last_EPOLL > it->second->last_EPOLL2)
                {
                    it->second->last_EPOLL2 = it->second->last_EPOLL;
                    std::cout << it->second->last_EPOLL << " seconds since last EPPOLLIN" << std::endl;;
                    if (it->second->last_EPOLL2 > 5)
                    {
                        int i = it->first;
                        free_node(it);
                        tree.erase(it);
                        return i;
                    }
                }
        }
        return 0;
    }

    bool check_suspended ( int file_descriptor )
    {
        std::map<int, client_buffer*>::iterator it = tree.find(file_descriptor);
        return it->second->state_suspended;
    }

    std::map<int, client_buffer*> get_tree ()
    {
        return tree;
    }

    private :

    //CONFIGURATION//

    std::map<int, client_buffer*> tree;

};

#endif
