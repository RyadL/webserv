#include "../includes/webserv.hpp"

MultipartParser::MultipartParser()
{
    std::cout << "MultipartParser default constructor" << std::endl;
}

MultipartParser::MultipartParser(const char* request_body, std::size_t body_length, std::string boundary)
    :_request_body(request_body), _body_length(body_length), _boundary("--" + boundary), _valid_part(true), _first_part(true)
{
    std::cout << "MultipartParser constructor 1" << std::endl;

}

MultipartParser::MultipartParser(FILE* file_request_body, std::size_t body_length, std::string boundary)
    :_file_request_body(file_request_body), _body_length(body_length), _boundary("--" + boundary), _valid_part(true), _first_part(true)
{
    std::cout << "MultipartParser constructor 2" << std::endl;
}

int MultipartParser::primary_check(std::size_t index)
{
    if (!std::strncmp(getRequestBody() + index + _boundary.length(), "\n", 1))
        return 1;
    if (!std::strncmp(getRequestBody() + index + _boundary.length(), "\r\n", 2))
        return 2; 
    return 0;
}

int MultipartParser::primary_check_long_body(std::size_t index, char *buf)
{
    if (!std::strncmp(buf + index + _boundary.length(), "\n", 1))
        return 1;
    if (!std::strncmp(buf + index + _boundary.length(), "\r\n", 2))
        return 2; 
    return 0;
}

int MultipartParser::further_validity_checks(std::size_t index)
{
    if (!std::strncmp(getRequestBody() + index - 1, "\n", 1))
        return 0;
    return 1;
}

int MultipartParser::further_validity_checks_long_body(std::size_t offset, char *buf)
{
    if (!std::strncmp(buf + offset - 1, "\n", 1))
        return 0;
    return 1;
}

std::size_t MultipartParser::find_boundary_start_short_body(std::size_t index)
{
    std::size_t add_index;
    std::cout << "FIND_BOUNDARY_START: " << std::endl;

    
    //std::cout << "EXIT667" << std::endl;
    //exit(0);
    for (; index < _body_length; index++)
    {
        if (!std::strncmp(getRequestBody() + index, _boundary.c_str(), _boundary.length()))
        {
            add_index = primary_check(index);
            if (_first_part)
            {
                if (add_index)
                {
                    _first_part = false;
                    return index + add_index + _boundary.length();
                }
            }
            // LE BOUNDARY DE DEBUT DOIT ETRE PRECEDE PAR UN '\n' POUR ETRE CONSIDERE COMME VALIDE
            else
            {
                if (add_index && !further_validity_checks(index))
                {
                    std::cout << "VALID BOUNDARY START FOUND" << std::endl;
                    return index + add_index + _boundary.length();
                }
            }
        }        
    }
    return _body_length;
}

std::size_t MultipartParser::find_boundary_start_long_body(std::size_t index)
{
    std::size_t add_index;
    char buf[4096];
    std::size_t ret;
    std::size_t offset = 0;
    std::cout << "FIND_BOUNDARY_START LONG: " << std::endl;

    std::cout << "index: " << index << " _body_length: " << _body_length << std::endl;
    std::cout << "getFileRequestBody(): " << getFileRequestBody() << std::endl;

    for (; index < _body_length; index++, offset++)
    {
        ret = read(fileno(getFileRequestBody()), buf + offset, 4096 - offset);
        std::cout << "START:";
        std::cout.write(buf, ret) << "END" << std::endl;
        if (_boundary.length() + 2 <= 4096 - offset)
        {
            if (!std::memcmp(buf + offset, _boundary.c_str(), _boundary.length()))
            {

                add_index = primary_check_long_body(offset, buf);
                std::cout << "ADD_INDEX: " << add_index << std::endl;

                
                if (_first_part)
                {
                    if (add_index)
                    {
                        _first_part = false;
                        return index + add_index + _boundary.length();
                    }
                }
                else
                {
                    if (add_index && !further_validity_checks_long_body(offset, buf))
                    {
                        std::cout << "VALID BOUNDARY START FOUND" << std::endl;
                        return index + add_index + _boundary.length();
                    }
                }

            }
        }
        else
        {
            std::memcpy(buf, buf + offset, 4096 - offset); // SI PAS SUFFISAMMENT DE PLACE POUR EFFECTUER LA COMPARAISON EN ENTIER AU DECALE LE RESTE VERSE LE DEBUT ET ON LIT LA SUITE POUR AVOIR SUFFISAMENT POUR TOUT COMPARER
            offset = 4096 - offset;
        }
        if (offset == 4096)
            offset = 0;
      
    }
    std::cout << "EXIT667974" << std::endl;
    exit(0);
    return _body_length;
}

std::size_t MultipartParser::parse_part(std::size_t start)
{
    bool    found_body_start = false;
    bool    found_body_end = false;
    std::size_t next_boundary;
    std::string content_disposition;
    std::size_t header_start;
    std::size_t header_end;
    std::size_t body_start;
    std::size_t body_end;

    next_boundary = start;
    bool    final_boundary = false;

    _header_start = start;
    if (start == _body_length)
        return _body_length;
    for (std::size_t    i = start; i < start + 20; i++)
        content_disposition += std::toupper(getRequestBody()[i]);

    if (std::strncmp(content_disposition.c_str(), "CONTENT-DISPOSITION:", 20))
    {
        std::cout << "INVALID PART" << std::endl;
        return next_boundary;
    }
    start += 21;
    // CHECKER NON DEPASSEMENT DE LA FIN
    for (; !found_body_start && start < _body_length; start++)
    {
        if (!std::strncmp(getRequestBody() + start, "\n\r\n", 3))
        {
            found_body_start = true;
            _header_end = start + 1;
            start += 2;
        }
        else if (!std::strncmp(getRequestBody() + start, "\n\n", 2))
        {
            found_body_start = true;
            _header_end = start + 1;
            start += 1;
        } 
    }
    _body_start = start;
    // CHECKER NON DEPASSEMENT DE LA FIN
    for (; !found_body_end && start < _body_length; start++)
    {
    
        if (!std::strncmp(getRequestBody() + start, _boundary.c_str(), _boundary.length()) && !std::strncmp(getRequestBody() + start - 1, "\n", 1))
        {
            found_body_end = true;
            if (!std::strncmp(getRequestBody() + start + _boundary.length(), "--", 2))
                final_boundary = true;
        }   
    }
    next_boundary = start - 1;
    _body_end = next_boundary;
    if (getRequestBody()[_body_end - 1] == '\n')
        _body_end--;
    if (getRequestBody()[_body_end - 1] == '\r')
        _body_end--;
    _headers_start_end.push_back(std::pair<std::size_t, std::size_t>(_header_start, _header_end));
    _bodies_start_end.push_back(std::pair<std::size_t, std::size_t>(_body_start, _body_end));
    if (final_boundary)
        return _body_length;
    return next_boundary;
}

// DONNER LE HEADER EN PARAMETRE
void    MultipartParser::parse_parts_header(std::size_t start_header, std::size_t end_header, std::size_t start_body, std::size_t end_body)
{
    std::ptrdiff_t i = 0;
    std::size_t save_start;

    char *header = new char[end_header - start_header + 1];
    std::memcpy((void*)header, getRequestBody() + start_header, end_header - start_header);
    header[end_header - start_header] = 0;
    MultipartPart   part(header);
    part.parse_header();
    part.setBodyStart(start_body);
    part.setBodyEnd(end_body);
    _parts.push_back(part);
}

int    MultipartParser::check_upload_validity(std::vector<MultipartPart> parts)
{
    std::vector<std::pair<std::string, std::string>>    headers;
    bool    filename;
    for (std::vector<MultipartPart>::const_iterator it1 = parts.begin(); it1 != parts.end(); it1++)
    {
        filename = false;
        headers = it1->getHeaders();
        for (std::vector<std::pair<std::string, std::string> >::const_iterator  it2 = headers.begin(); it2 != headers.end(); it2++)
        {
            if (it2->first == "filename")
            {
                filename = true;
                break;
            }
        }
        if (filename == false)
            return 0;
    }
    return 1;
}

std::size_t MultipartParser::getBodyLength() const
{
    return _body_length;
}

std::string MultipartParser::getBoundary() const
{
    return _boundary;
}

const char  *MultipartParser::getRequestBody() const
{
    return _request_body;
}

FILE    *MultipartParser::getFileRequestBody() const
{
    return _file_request_body;
}

std::vector<std::pair<std::size_t, std::size_t> >   MultipartParser::getHeadersStartEnd() const
{
    return _headers_start_end;
}

std::vector<std::pair<std::size_t, std::size_t> >   MultipartParser::getBodiesStartEnd() const
{
    return _bodies_start_end;
}

std::vector<MultipartPart>  MultipartParser::getParts() const
{
    return _parts;
}