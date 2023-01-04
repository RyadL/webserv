#ifndef MULTIPARTPARSER_HPP
# define MULTIPARTPARSER_HPP

//#include "../includes/webserv.hpp"
#include "MultipartPart.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <cstring>

class   MultipartParser
{
private:
    const char  *_request_body;
    std::size_t _body_length;
    std::string _boundary;

    std::vector<MultipartPart>  _parts;
    std::vector<std::pair<std::size_t, std::size_t> >   _headers_start_end;
    std::vector<std::pair<std::size_t, std::size_t> >   _bodies_start_end;
    //std::vector<std::pair<std::string, std::string> >   _headers_fields;
    // FAIRE UNE CLASSE MULTIPART PART ET FAIRE UN VECTOR DE PARTS
    // PARTS CONTIENDRA UN VECTOR DE HEADERS
    std::size_t _header_start;
    std::size_t _header_end;
    std::size_t _body_start;
    std::size_t _body_end;

    bool    _valid_part;
    bool    _first_part;

    int primary_check(std::size_t index);
public:
    MultipartParser();
    MultipartParser(const char*, std::size_t, std::string);

    std::size_t find_boundary_start(std::size_t);
    //int primary_check(std::size_t index);
    std::size_t parse_part(std::size_t start);
    void    parse_parts_header(std::size_t, std::size_t, std::size_t, std::size_t);
    int    check_upload_validity(std::vector<MultipartPart>);
    int further_validity_checks(std::size_t);

    std::size_t getBodyLength() const;
    std::string getBoundary() const;
    const char  *getRequestBody() const;

    std::vector<std::pair<std::size_t, std::size_t> >   getHeadersStartEnd() const;
    std::vector<std::pair<std::size_t, std::size_t> >   getBodiesStartEnd() const;
    std::vector<MultipartPart>  getParts() const;
};

#endif