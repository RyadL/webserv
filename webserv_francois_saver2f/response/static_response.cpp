#include "static_response.hpp"

Static_response::Static_response()
{}

Static_response::~Static_response()
{
    delete [] _buffer;
}

Static_response::Static_response(Request_parser req_pars, int fd)
    :_req_pars(req_pars), _http_version("HTTP/1.1"), _fd(fd), _allowed_method(true)
{
    std::cout << "Static_response constructor" << std::endl;
}

void    Static_response::create_response_header()
{
    
}

std::string Static_response::getStatusCode()
{
    return _status_code;
}

void    Static_response::setStatusCode(const char* status_code)
{
    _status_code = status_code;
}

std::string Static_response::getReasonPhrase()
{
    return _reason_phrase;
}
void    Static_response::setReasonPhrase(const char* reason_phrase)
{
    _reason_phrase = reason_phrase;
}


// PARSER ROOT DIRECTIVE POUR QUE LA FIN NE SOIT PAS / NGINX ACCEPTE LES DEUX MAIS PARSE POUR QUE LES 2 CAS FONCTIONNENT
void    Static_response::execute_request()
{
    std::ifstream   file;
    std::string     file_path(_req_pars.getRootDirectory());
    struct stat sb;
    
    
    std::cout << "_req_pars.getRequestUri(): " << _req_pars.getRequestUri() << std::endl;
    std::cout << "_req_pars.getTitle(): " << _req_pars.getTitle() << std::endl;
    //if (_req_pars.getRequestUri() == "/" || _req_pars.getRequestUri() == _req_pars.getTitle())
    //if (_req_pars.getRequestUri() == _req_pars.getTitle())
    if (file_path[file_path.length() - 1] != '/')
        file_path += "/";

    if (_req_pars.getRequestUri() != _req_pars.getTitle())
    {
        if (_req_pars.getRequestUri()[_req_pars.getTitle().length()] == '/')
            file_path += _req_pars.getRequestUri().substr(_req_pars.getTitle().length() + 1, std::string::npos);
        else
            file_path += _req_pars.getRequestUri().substr(_req_pars.getTitle().length(), std::string::npos);
    }
    std::cout << "FILE_PATH0: " << file_path << std::endl;
    if (stat(file_path.c_str(), &sb) == 0 && (sb.st_mode & S_IFDIR)) 
    {
        std::cout << "IS A DIRECTORY" << std::endl;
        if (file_path[file_path.length() - 1] != '/')
            file_path += "/";
        file_path += _req_pars.getIndex();
    }
    else
        std::cout << "NOT A DIRECTORY" << std::endl;
    std::cout << "FILE_PATH1: " << file_path << std::endl;
    /*
    if (!_req_pars.getRequestUri().find(_req_pars.getTitle()))
    {
        std::cout << "FILE_PATH0: " << file_path << std::endl;
        if (_req_pars.getRequestUri() == "/")
            file_path += "/";
        else
            file_path += _req_pars.getRequestUri().substr(_req_pars.getTitle().length(), std::string::npos);
        std::cout << "FILE_PATH1: " << file_path << std::endl;
        if (stat(file_path.c_str(), &sb) == 0 && (sb.st_mode & S_IFDIR)) 
        {
            file_path += _req_pars.getIndex();
        }
        
        
        file_path += "/";

        if (_req_pars.getRequestUri() == _req_pars.getTitle())
        {
            if (_req_pars.getIndex().empty())
                file_path += "index.html";
            else
                file_path += _req_pars.getIndex();
        }
        else
            file_path += _req_pars.getRequestUri().substr(_req_pars.getTitle().length() + 1, std::string::npos);

        
    }
    */
    //else if (_req_pars.getRequestUri() == )
    //else
    //    file_path += _req_pars.getRequestUri().substr(_req_pars.getTitle().length(), std::string::npos);
    file.open(file_path.c_str());
    std::cout << "FILE_PATH: " << file_path << std::endl;
    if (!file.is_open())
    {
        
        error_response(404);
        
        std::cout << "FAILED TO OPEN THE FILE" << std::endl; // 403 Forbidden if index == true
        return;
    }
    /*
    struct stat sb;
    if (stat(file_path.c_str(), &sb) == 0 && (sb.st_mode & S_IFDIR)) 
    {
        
        error_response(404);
        
        std::cout << "FAILED TO OPEN THE FILE" << std::endl; // 403 Forbidden if index == true
        return;
    }
    */
    file.ignore(std::numeric_limits<std::streamsize>::max());
    _length = file.gcount();
    file.clear();
    file.seekg(0, std::ios_base::beg);

    std::string response_line("HTTP/1.1");
    response_line += " ";
    response_line += "200";
    response_line += " ";
    response_line += "OK";
    response_line += "\r\n";

    std::string content_length("Content-Length: ");
    std::ostringstream ss;
    ss << _length;
    content_length += ss.str();
    content_length += "\r\n\r\n";
    _buffer = new char[response_line.length() + content_length.length() + _length + 1];
    std::memcpy(_buffer, response_line.c_str(), response_line.length());
    std::memcpy(_buffer + response_line.length(), content_length.c_str(), content_length.length());
    file.read(_buffer + response_line.length() + content_length.length(), _length);
    _length += response_line.length() + content_length.length();
    _buffer[_length] = 0;
    //std::cout << "BUFFER667:" << std::endl;
    //std::cout << _buffer << std::endl;
}

void    Static_response::open_file_and_write(std::string filename, const char* body, std::size_t len)
{
    std::ofstream file_Upload(filename.c_str(), std::ios::out | std::ios::binary);
    if (!file_Upload.is_open())
    {
        std::cout << "FILE FAILED TO OPEN" << std::endl;
        return;
    }
    file_Upload.write(body, len);
}

void    Static_response::execute_uploading(std::vector<MultipartPart> parts, const char* body)
{
    std::vector<std::pair<std::string, std::string>>    headers;
    for (std::vector<MultipartPart>::const_iterator it1 = parts.begin(); it1 != parts.end(); it1++)
    {
        headers = it1->getHeaders();
        for (std::vector<std::pair<std::string, std::string> >::const_iterator  it2 = headers.begin(); it2 != headers.end(); it2++)
        {
            if (it2->first == "filename")
            {
                open_file_and_write(it2->second, body + it1->getBodyStart(), it1->getBodyEnd() - it1->getBodyStart());
                break;
            }
                
        }
    }
    return;    
}

int Static_response::retrieve_boundary(std::string value_field)
{
    std::size_t found;

    std::string upper_value_field;
    for (std::string::const_iterator    it = value_field.begin(); it != value_field.end(); it++)
        upper_value_field += std::toupper(*it);
    //std::cout << "upper_value_field: " << upper_value_field << std::endl;
    
    found = upper_value_field.find("MULTIPART/FORM-DATA;");
    if (found == std::string::npos)
        found = upper_value_field.find("MULTIPART/FORM-DATA ");
    if (found == std::string::npos)
    {
        //std::cout << "ERROR Bad Request" << std::endl;
        return 1;
    }
    found += 21;
    found = upper_value_field.find("BOUNDARY", found);
    if (found == std::string::npos)
    {
        //std::cout << "ERROR Bad Request" << std::endl;
        return 1;        
    }
    found += 9;
    upper_value_field.find("=", found);
    if (found == std::string::npos)
    {
        //std::cout << "ERROR Bad Request" << std::endl;
        return 1;        
    }
    std::string::const_iterator end_boundary = value_field.end();
    end_boundary -= 2;
    _boundary = value_field.substr(found, value_field.length() - 2);
    return 0;
}

std::ptrdiff_t Static_response::check_multipart_header()
{
    std::vector<std::pair<std::string, std::string> > headers = getReqpars().getRequestHeaders();  
                                    
    std::vector<std::pair<std::string, std::string> >::const_iterator   it = headers.begin();
    for (; it != headers.end(); it++)
    {
        if (it->first == "CONTENT_TYPE")
            break;    
    }
    return (it - headers.begin());
}

std::size_t Static_response::parse_part(std::size_t start)
{
    bool    found_body_start = false;
    bool    found_body_end = false;
    std::size_t body_length = 38639;
    std::size_t header_start;
    std::size_t header_end;
    std::size_t body_start;
    std::size_t body_end;
    std::size_t next_boundary;
    //std::string content_disposition(_req_pars.getBodyBuffer(), start, 21);
    std::string content_disposition;
    next_boundary = start;
    bool    final_boundary = false;

    header_start = start;
    if (start == body_length)
        return body_length;
    for (std::size_t    i = start; i < start + 20; i++)
        content_disposition += std::toupper(_req_pars.getBodyBuffer()[i]);

    if (std::strncmp(content_disposition.c_str(), "CONTENT-DISPOSITION:", 20))
    {
        //std::cout << "INVALID PART" << std::endl;
        //std::cout << "_req_pars.getBodyBuffer() + start:\n" << _req_pars.getBodyBuffer() + start << std::endl;
        return next_boundary;
    }
    start += 21;
    // CHECKER NON DEPASSEMENT DE LA FIN
    for (; !found_body_start; start++)
    {
        if (!std::strncmp(_req_pars.getBodyBuffer() + start, "\n\r\n", 3))
        {
            found_body_start = true;
            header_end = start;
            start += 2;
        }
        else if (!std::strncmp(_req_pars.getBodyBuffer() + start, "\n\n", 2))
        {
            found_body_start = true;
            header_end = start;
            start += 1;
        } 
    }
    body_start = start;
    // CHECKER NON DEPASSEMENT DE LA FIN
    for (; !found_body_end; start++)
    {
        if (!std::strncmp(_req_pars.getBodyBuffer() + start, _boundary.c_str(), _boundary.length()) && !std::strncmp(_req_pars.getBodyBuffer() + start - 1, "\n", 1))
        {
            found_body_end = true;
            if (!std::strncmp(_req_pars.getBodyBuffer() + start + _boundary.length(), "--", 2))
                final_boundary = true;
        }
             
    }
    next_boundary = start - 1;
    body_end = next_boundary;
    if (_req_pars.getBodyBuffer()[body_end - 1] == '\n')
        body_end--;
    if (_req_pars.getBodyBuffer()[body_end - 1] == '\r')
        body_end--;
    if (final_boundary)
        return body_length;
    return next_boundary;
}

void    Static_response::find_part()
{
    std::size_t  body_length = 38639;
    std::size_t index = 0;
    std::size_t end_found;
    int  counter = 0;

    MultipartParser  mult_pars(_req_pars.getBodyBuffer(), body_length, _boundary);

    while (index < body_length)
    {
        index = mult_pars.find_boundary_start(index);
        index = mult_pars.parse_part(index);
    }

    std::vector<std::pair<std::size_t, std::size_t> > header = mult_pars.getHeadersStartEnd();
    std::vector<std::pair<std::size_t, std::size_t> > body = mult_pars.getBodiesStartEnd();
    for (std::size_t    i = 0; i < header.size(); i++)
        mult_pars.parse_parts_header(header[i].first, header[i].second, body[i].first, body[i].second);
    std::vector<MultipartPart>  parts = mult_pars.getParts();
    if (mult_pars.check_upload_validity(parts))
    {
        //std::cout << "UPLOAD INVALID" << std::endl;
        return;
    }
    execute_uploading(parts, _req_pars.getBodyBuffer()); 
}

// \r\n N'EST PAS INCLUS DANS LE BOUNDARY DONC ON VA CHERCHER LES LIGNES DANS LE BODY (strncmp("\n", 1)) ET ON VA COMPARER CE QUI VIENT AVANT LE "\n" AVEC LES BOUNDARY

// ON VA RETOURNER ET RECUPERER LA PTR_DIFF ENTRE IT ET BEGIN

void    Static_response::upload_file()
{
    std::ptrdiff_t header = check_multipart_header();
    if (header == _req_pars.getRequestHeaders().size())
    {
        //std::cout << "ERROR Bad Request" << std::endl;
        return;
    }
    if (retrieve_boundary(_req_pars.getRequestHeaders()[header].second))
        return;
    find_part(); 
}

void    Static_response::prepare_cgi_response(char *buffer, std::size_t length)
{
    if (!_response_headers.empty())
        _response_headers.clear();


    std::string test(buffer);
    std::size_t found1 = test.find("\r\n\r\n");
    std::size_t found2 = 0;
    std::string test2 = test.substr(0, found1 + 2);

    std::string status_line("HTTP/1.1");
    std::string code;
    std::string reason_phrase;

    std::string header_field;
    std::size_t found5;
    while (found2 < found1)
    {
        std::size_t found3 = test2.find("\r\n", found2);
        header_field = test2.substr(found2, found3 - found2);
        std::size_t found4 = header_field.find(":");
        if (found4 != std::string::npos)
        {
            found5 = header_field.find("Status:");
            if (found5 != std::string::npos)
            {
                code = header_field.substr(found5 + 8, 3);
                //std::cout << "CODE: " << code << std::endl;
                reason_phrase = header_field.substr(found5 + 12, std::string::npos);
            }
            else
            {

                //header_field length
                std::pair<std::string, std::string> tmp(header_field.substr(0, found4), header_field.substr(found4 + 2, std::string::npos));
                _response_headers.push_back(tmp);
            }
                
        
        }
        found2 = found3 + 2;
    }
    //std::cout << "status_line: " << status_line << std::endl;
    if (code.empty())
    {
        code = "200";
        reason_phrase = "OK";
    }
        
    
    //std::cout << "code: " << code << std::endl;
    //std::cout << "reason_phrase: " << reason_phrase << std::endl;
    //std::cout << "header_field: " << header_field << std::endl;
    // IF CODE == 404 THEN LOAD ERROR DEFAULT PAGE IN BODY

    status_line += " ";
    status_line += code;
    status_line += " ";
    status_line += reason_phrase;
    status_line += "\r\n";
    std::string content_length("Content-Length: ");
    std::ostringstream ss;
    //std::cout << "LENGTH1: " << length - found1 << std::endl;
    ss << length - found1;
    content_length += ss.str();
    content_length += "\r\n\r\n";

    // IL FAUDRA CALCULER HEADER_FIELD_LENGTH AU MEME MOMENT OU ON CREE LE VECTOR RESPONSE_HEADERS
    std::size_t header_field_length = 0;
    for (std::vector<std::pair<std::string, std::string> >::iterator it = _response_headers.begin(); it != _response_headers.end(); it++)
    {
        header_field_length += (it->first.length() + it->second.length() + 4);
    }
    //    
    
    _buffer = new char[status_line.length() + header_field_length + content_length.length() + length - found1 - 4 + 1];
    std::memcpy(_buffer, status_line.c_str(), status_line.length());
    std::size_t len = 0;
    for (std::vector<std::pair<std::string, std::string> >::iterator it = _response_headers.begin(); it != _response_headers.end(); it++)
    {
        std::string line(it->first);
        line += ":";
        line += " ";
        line += it->second;
        line += "\r\n";
        std::memcpy(_buffer + status_line.length() + len, line.c_str(), line.length());
        len += line.length();
    }    
    std::memcpy(_buffer + status_line.length() + len, content_length.c_str(), content_length.length());
    std::memcpy(_buffer + status_line.length() + len + content_length.length(), buffer + found1 + 4, length - found1 - 4);
    _length = status_line.length() + len + content_length.length() + length - found1 - 4;
    _buffer[status_line.length() + len + content_length.length() + length - found1 - 4] = 0;

}

void    Static_response::setRequestParser(Request_parser req_pars)
{
    _req_pars = req_pars;
}

void    Static_response::error_response(int code)
{
    std::string status_code;
    std::string reason_phrase;

    if (code == 404)
    {
        status_code = "404";
        reason_phrase = "Not Found";
    }
    /*
    if (code == 405 || code == 667)
    {
        status_code = "405";
        reason_phrase = "Method Not Allowed";
    }
    */
    if (code == 666)
    {
        status_code = "405";
        reason_phrase = "Method Not Allowed";        
    }
    if (code == 667)
    {
        std::cout << "ON EST LA" << std::endl;
        status_code = "405";
        reason_phrase = "Method Not Allowed";        
    }
    std::string response_line("HTTP/1.1");
    response_line += " ";
    response_line += status_code;
    response_line += " ";
    response_line += reason_phrase;
    response_line += "\r\n";

    std::string body("<html>\r\n<head><title>");
    body += status_code;
    body += " ";
    body += reason_phrase;
    body += "</title></head>\r\n<body>\r\n<center><h1>";
    body += status_code;
    body += " ";
    body += reason_phrase;
    // REMPLACER NGINX PAR SERVER NAME
    body += "</h1></center>\r\n<hr><center>nginx/1.18.0 (Ubuntu)</center>\r\n</body>\r\n</html>\r\n";
    std::string content_length("Content-Length: ");
    std::stringstream   str_convert;
    str_convert << body.length();
    content_length += str_convert.str();
    content_length += "\r\n\r\n";
    _buffer = new char[response_line.length() + content_length.length() + body.length() + 1];
    std::memcpy(_buffer, response_line.c_str(), response_line.length());
    std::memcpy(_buffer + response_line.length(), content_length.c_str(), content_length.length());
    std::memcpy(_buffer + response_line.length() + content_length.length(), body.c_str(), body.length());
    if (code == 667)
        //_length = response_line.length() + content_length.length();
        _length = response_line.length() + content_length.length() + body.length();
    else
        _length = response_line.length() + content_length.length() + body.length();
    _buffer[_length] = 0;
    //std::cout << "_BUFFER: " << _buffer << std::endl;
}
// GESTION DE _LENGTH OPTIMISABLE POUR EVITER DE REFAIRE LE CALCUL DES SOMMES

int Static_response::getFd()
{
    return _fd;
}

void    Static_response::setFd(int fd)
{
    _fd = fd;
}

char *Static_response::getBuffer() const
{
    return _buffer;
}

std::streamsize Static_response::getLength() const
{
    return _length;
}

Request_parser  Static_response::getReqpars() const
{
    return _req_pars;
}

std::string Static_response::getBoundary() const
{
    return _boundary;
}

void    Static_response::setAllowedMethod(bool allowed_methods)
{
    _allowed_method = allowed_methods;
}

bool    Static_response::getAllowedMethods() const
{
    return _allowed_method;
}



// LE PARSER LIT DU DEBUT VERS LA FIN A LA RECHERCHE D'UNE CORRESPONDANCE AVEC LE BOUNDARY
// LE BOUNDARY DOIT ETRE EXACTEMENT IDENTIQUE MAIS AU PREND EN COMPTE LES CARACTERES QUI LE SUIVENT
// SEULEMENT POUR UN BOUNDARY DE DEBUT DE PARTIE.
// UN BOUNDARY DE FIN DE PARTIE NE SERA PAS IMPACTER ET LE FICHIER SERA CONSIDERE COMME VALIDE AVEC
// UN DEBUT ET UNE FIN
// UN BOUNDARY DE DEBUT DE PARTIE S'IL N'EST PAS SUIVI DIRECTEMENT D'UN '\r\n' OU D'UN '\n' ET EGALEMENT PRECEDE D'UN '\n' SERA CONSIDERE
// COMME ETANT INVALIDE ET DONC LA PARTIE SERA INVALIDE ET ON PASSERA A LA PARTIE SUIVANTE
// LE BOUNDARY-- N'EST PAS REELLEMENT SIGNIFICATIF
// LE PARSER NE PEUT PAS SE FINIR EN ETANT A LA RECHERCHE D'UN BOUNDERY DE DEBUT VALIDE
// IL DOIT OBLIGATOIREMENT AVOIR UN BOUNDERY DE DEBUT VALIDE ET ETRE A LA RECHERCHE D'UN BOUNDERY DE FIN
// CE BOUNDERY DE FIN PEUT ETRE TOTALEMENT INVALIDE

// IL DOIT OBLIGATOIREMENT Y AVOIR UNE PART SANS FILENAME ("BOUTON") ET CELLE SI PEUT MAL SE TERMINER
// CELLE-CI PEUT NE PAS SE SITUER NECESSAIREMENT A LA FIN DU BODY MAIS ELLE EST OBLIGATOIRE
// SI LE PARSER RENCONTRE LE BOUNDARY-- IL CONSIDERE QUE LE BODY EST TERMINE


// 1) ON CHERCHE UN BOUNDARY DE DEBUT DE PART VALIDE
// 2) ON CHERCHE UN BOUNDARY DE FIN
// 3) UN REPETE AVEC TOUTES LES PARTS
// 4) ON PARSE LES HEADERS DES PARTS ET ON VERIFIE QU'AU MOINS UNE PART NE DISPOSE PAS DE "FILENAME"

// TOUTE PART AYANT UN BOUNDARY DE DEBUT INVALIDE SERA SAUTER POUR LE SUIVANT


