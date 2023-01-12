#include "../includes/webserv.hpp"

Static_response::Static_response()
    :_rsp_buf_tree_ptr(&_rsp_buf_tree)
{
    std::cout << "Static response default constructor" << std::endl;
    std::cout << "LA0: " << std::endl;
    std::cout << "_rsp_buf_tree_ptr: " << _rsp_buf_tree_ptr << std::endl;
    std::cout << "&_rsp_buf_tree_ptr: " << &_rsp_buf_tree_ptr << std::endl;
}

Static_response::~Static_response()
{
    //delete [] _buffer;
}

void    Static_response::add_client(int fd)
{
    Response_buffers *test = new Response_buffers;
    std::pair<int, Response_buffers*> new_client(fd, test);
    _rsp_buf_tree.insert(new_client);
    std::cout << "&_rsp_buf_tree.find(fd)->second: " << &_rsp_buf_tree.find(fd)->second << std::endl;
    std::cout << "&_rsp_buf_tree_ptr->find(fd)->second << std::endl: " << &_rsp_buf_tree_ptr->find(fd)->second << std::endl;

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

void    Static_response::fill_buf_body_static(std::ifstream *file, std::size_t offset, std::size_t bytes_count)
{
    std::size_t i = 0;
    char    buf[4096];
    
    Response_buffers *resp_buf = _rsp_buf_tree_ptr->find(_fd)->second;
    while (bytes_count && i < 8)
    {
        if (bytes_count > 4096 - offset)
        {
            file->read((*resp_buf->getBuffersSetPtr())[i].first + offset, 4096 - offset); 
            bytes_count -= 4096 - offset;
            resp_buf->add_buffer();
            i++;
        }
        else
        {
            file->read((*resp_buf->getBuffersSetPtr())[i].first + offset, bytes_count);
            bytes_count = 0;
        }
    }
    if (bytes_count)
    {
        while (bytes_count)
        {
            if (bytes_count > 4096)
            {
                file->read(buf, 4096);
                write(fileno(resp_buf->getTmpFile()), buf, 4096);
                bytes_count -= 4096;
            }
            else
            {
                file->read(buf, bytes_count);
                write(fileno(resp_buf->getTmpFile()), buf, bytes_count);
                bytes_count = 0;
            }
        }
    }
}

void    Static_response::execute_request()
{
    std::ifstream   file;
    std::string     file_path(_req_pars.getRootDirectory());
    struct stat sb;
    
    if (file_path[file_path.length() - 1] != '/')
        file_path += "/";

    if (_req_pars.getRequestUri() != _req_pars.getTitle())
    {
        if (_req_pars.getRequestUri()[_req_pars.getTitle().length()] == '/')
            file_path += _req_pars.getRequestUri().substr(_req_pars.getTitle().length() + 1, std::string::npos);
        else
            file_path += _req_pars.getRequestUri().substr(_req_pars.getTitle().length(), std::string::npos);
    }
    if (stat(file_path.c_str(), &sb) == 0 && (sb.st_mode & S_IFDIR)) 
    {
        if (file_path != _req_pars.getRootDirectory() + "/")
        {
            error_response(403);
            return ;
        }
        if (file_path[file_path.length() - 1] != '/')
            file_path += "/";
        file_path += _req_pars.getIndex();
    }
    std::cout << "FILE_PATH: " << file_path << std::endl;
    file.open(file_path.c_str());
    if (!file.is_open())
    {
        error_response(404);
        return;
    }
    file.ignore(std::numeric_limits<std::streamsize>::max());
    std::size_t length = file.gcount();
    file.clear();
    file.seekg(0, std::ios_base::beg);
    std::string response_line("HTTP/1.1");
    response_line += " ";
    response_line += "200";
    response_line += " ";
    response_line += "OK";
    response_line += "\r\n";

    Response_buffers    *resp_buf = _rsp_buf_tree_ptr->find(_fd)->second;
    std::vector<std::pair<char *, std::size_t>> *buf_set = resp_buf->getBuffersSetPtr();
    std::string content_length("Content-Length: ");
    std::ostringstream ss;
    ss << length;
    content_length += ss.str();
    content_length += "\r\n\r\n";
    resp_buf->add_buffer();
    std::memcpy((*buf_set)[0].first, &response_line[0], response_line.length());
    std::memcpy((*buf_set)[0].first + response_line.length(), &content_length[0], content_length.length());
    
    fill_buf_body_static(&file, response_line.length() + content_length.length(), length);
    resp_buf->setCountBytes(response_line.length() + content_length.length() + length);
    file.close();
}

std::string    Static_response::clean_upload_filename(std::string filename)
{
    std::cout << "CLEAN_UPLOAD_FILENAME" << std::endl;


    if (filename.length() > 2 && filename[0] == '\"' && filename[filename.length() - 1] == '\"')
        return filename.substr(1, filename.length() - 2);
    return filename;
}

void    Static_response::open_file_and_write(std::string filename, const char* body, std::size_t len)
{
    std::cout << "filename0: " << filename << " et len: " << len << std::endl;
    filename = clean_upload_filename(filename);
    std::cout << "filename1: " << filename << " et len: " << len << std::endl;

    if (filename == "\"\"")
    {
        std::cout << "NO FILE" << std::endl;
        return ;
    }
    
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
        error_response(400);
        return 1;
    }
    found += 21;
    found = upper_value_field.find("BOUNDARY", found);
    if (found == std::string::npos)
    {
        error_response(400);
        return 1;        
    }
    found += 9;
    upper_value_field.find("=", found);
    if (found == std::string::npos)
    {
        error_response(400);
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
        std::cout << "it->first: " << it->first << std::endl;
        if (it->first == "CONTENT_TYPE")
        {
            std::cout << "ON EST LA" << std::endl;

            break;
        }
                
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

    std::cout << "_req_pars.getIsShortBody(): " << _req_pars.getIsShortBody() << std::endl;
    std::cout << "_req_pars.getIsLongBody(): " << _req_pars.getIsLongBody() << std::endl;
    if (_req_pars.getIsShortBody())
    {
        MultipartParser  mult_pars(_req_pars.getBodyBuffer(), _req_pars.getBytesInsideBody(), _boundary);
        while (index < _req_pars.getBytesInsideBody())
        {
            index = mult_pars.find_boundary_start_short_body(index);
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
    if (_req_pars.getIsLongBody())
    {

        std::cout << "OKAY0" << std::endl;
        std::cout << "_req_pars.getFileLongBody(): " << _req_pars.getFileLongBody() << std::endl;

        MultipartParser  mult_pars(_req_pars.getFileLongBody(), _req_pars.getBytesInsideFileBody(), _boundary);
        
        while (index < _req_pars.getBytesInsideFileBody())
        {
            index = mult_pars.find_boundary_start_long_body(index);
            //std::cout << "EXIT974" << std::endl;
            //exit(0);
            //index = mult_pars.parse_part(index);
        }
    }
        
    //MultipartParser  mult_pars(_req_pars.getBodyBuffer(), body_length, _boundary);
    //std::cout.write(_req_pars.getBodyBuffer(), _req_pars.getBytesInsideBody()) << "END" << std::endl;
    std::cout << "ON EST LA" << std::endl;
    std::cout << "EXIT667" << std::endl;
    exit(0);
    /*
    while (index < body_length)
    {
        index = mult_pars.find_boundary_start_short_body(index);
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
    */

}

// \r\n N'EST PAS INCLUS DANS LE BOUNDARY DONC ON VA CHERCHER LES LIGNES DANS LE BODY (strncmp("\n", 1)) ET ON VA COMPARER CE QUI VIENT AVANT LE "\n" AVEC LES BOUNDARY

// ON VA RETOURNER ET RECUPERER LA PTR_DIFF ENTRE IT ET BEGIN

void    Static_response::upload_file()
{


    std::ptrdiff_t header = check_multipart_header();
    std::cout << "oui" << std::endl;

    //std::cout.write(_req_pars.getBodyBuffer(), _req_pars.getBytesInsideBody()) << "END667" << std::endl;
    
    if (header == _req_pars.getRequestHeaders().size())
    {

        error_response(400); // ATTENTION A BIEN ARRETER LE CODE ENSUITE
        return;
    }

    std::cout << "ouioui" << std::endl;


    
    if (retrieve_boundary(_req_pars.getRequestHeaders()[header].second))
        return;

    find_part(); 

    
}

std::size_t Static_response::find_end_header_field(Response_buffers *rsp_buf, std::size_t offset, std::size_t bytes_count, std::size_t index)
{
    std::cout << "FIND_END_HEADER_FIELD" << std::endl;
    std::size_t header_field_len = 0;

    std::cout << "offset: " << offset << std::endl;
    std::cout << "bytes_count: " << bytes_count << std::endl;
    std::cout << "index: " << index << std::endl;
    while (header_field_len < bytes_count && index < 8)
    {
        if (offset > 4096)
        {
            index++;
            offset = 0;
        }
        //std::cout << "(*rsp_buf->getBuffersSetPtr())[index].first[offset]: " << (*rsp_buf->getBuffersSetPtr())[index].first[offset] << std::endl;
        if (!std::memcmp((*rsp_buf->getBuffersSetPtr())[index].first + offset, "\n", 1))
        {
            if (!std::memcmp((*rsp_buf->getBuffersSetPtr())[index].first + offset - 1, "\r", 1))
                header_field_len--;
            std::cout << "HEADER_FIELD_LEN: " << header_field_len << std::endl;

            break ;
        }
        //std::cout << "header_field_len0: " << header_field_len << std::endl;
        header_field_len++; 
        offset++;
    }
    //std::cout << "header_field_len1: " << header_field_len << std::endl;
    return header_field_len;
}

char*    Static_response::generate_header_field(Response_buffers *rsp_buf, std::size_t *offset, std::size_t *index, std::size_t len)
{
    std::cout << "GENERATE_HEADER_FIELD" << std::endl;
    std::size_t i = 0;
    char *buf = new char[len + 2];
    std::size_t j = *offset;

    std::size_t save_len = len;

    std::memcpy(buf + len, "\r\n", 2);
    while (len && *index < 8)
    {
        if (len > 4096 - j)
        {
            std::memcpy(buf + i, (*rsp_buf->getBuffersSetPtr())[*index].first + j, 4096 - j);
            (*index)++;
            i += 4096 - j;
            len -= 4096 - j;
            j = 0;
        }
        else
        {
            std::memcpy(buf + i, (*rsp_buf->getBuffersSetPtr())[*index].first + j, len);
            i += len;
            len = 0;
            j += len;
        }
    }
    *offset = j;
    // TMPFILE A GERER ? JE PENSE PAS CAR I > 8 ARRIVERA AVANT j > 8 DANS TOUS LES CAS
    std::cout.write(buf, save_len + 2) << "END667" << std::endl;
    std::cout << "save_len + 2: " << save_len + 2 << std::endl;
    return buf;
}

void    Static_response::add_cgi_header(std::size_t bytes_count, std::size_t offset_start, Response_buffers *resp_buf, Response_buffers *rsp_buf)
{
    std::cout << "ADD_CGI_HEADER" << std::endl;
    std::pair<std::size_t, std::size_t> offsets(offset_start, 0);
    std::size_t i = resp_buf->getIndex() - 1;
    std::size_t j = 0;
    std::size_t ret = 1;
    char    buf[4096];
    std::size_t found = 0;
    std::size_t header_field_len;
    std::size_t count_bytes;
    std::size_t save_bytes_count = bytes_count;
    std::cout << "INDEX1: " << resp_buf->getIndex() << std::endl;
    //std::cout.write((*rsp_buf->getBuffersSetPtr())[j].first, 42) << "END" << std::endl;
    //std::cout << "EXIT667" << std::endl;
    //exit(0);
    while (bytes_count > 0 && i < 8)
    {
        std::cout << "i: " << i << std::endl;
        std::cout << "(*resp_buf->getBuffersSetPtr()).size(): " << (*resp_buf->getBuffersSetPtr()).size() << std::endl;
        std::cout << "(*rsp_buf->getBuffersSetPtr()).size(): " << (*rsp_buf->getBuffersSetPtr()).size() << std::endl;
        std::cout << "(*resp_buf->getBuffersSetPtr())[i].first: " << (*resp_buf->getBuffersSetPtr())[i].first << std::endl;
        std::cout << "(*rsp_buf->getBuffersSetPtr())[j].first: " << (*rsp_buf->getBuffersSetPtr())[j].first << std::endl;
        header_field_len = find_end_header_field(rsp_buf, offsets.second, save_bytes_count, j);

        std::cout << "header_field_len: " << header_field_len << std::endl;

        char *test = generate_header_field(rsp_buf, &offsets.second, &j, header_field_len);
        offsets = fill_buf_header((*resp_buf->getBuffersSetPtr())[i].first, test, &header_field_len, offsets);
        if (bytes_count < offsets.second)
            bytes_count = 0;
        else
            bytes_count -= offsets.second;
        if (!offsets.second)
        {
            offsets.second = 0;
            j++;
        }
        if (!offsets.first)
        {
            i++;
            resp_buf->add_buffer();
        }
    }
    if (bytes_count)
    {
        i++;
        fill_tmpfile(std::pair<Response_buffers*, Response_buffers*>(resp_buf, rsp_buf), bytes_count, j, offsets.second);
    }
        
    std::cout << "offsets.first: " << offsets.first << " et offsets.second: " << offsets.second << std::endl;
    resp_buf->setIndex(i);
     std::cout << "offsets.first667: " << offsets.first << " et offsets.second: " << offsets.second << std::endl;
    resp_buf->setHeaderEnd(offsets.first);
    //std::cout << "EXIT667" << std::endl;
    //exit(0);
}

void    Static_response::print_response_header(std::vector<std::pair<char*, std::size_t> >  *vect_bufs, FILE *tmpfile,  std::size_t bytes_count)
{
    std::cout << "PRINT_RESPONSE_HEADER" << std::endl;
    char    buf[4096];
    std::size_t ret = 1;

    for (std::size_t    i = 0; bytes_count && i < 8; i++)
    {
        std::cout << "BYTES_COUNT: " << bytes_count << std::endl;
        if (bytes_count < 4096)
        {
            std::cout.write((*vect_bufs)[i].first, bytes_count);
            return ;
        }   
        else
        {
            std::cout.write((*vect_bufs)[i].first, 4096);
            bytes_count -= 4096;
        }

    }
    std::cout << "BYTES_COUNT667: " << bytes_count << std::endl;
    if (bytes_count)
    {
        while (ret)
        {
            ret = read(fileno(tmpfile), buf, 4096);
            std::cout << "RET: " << ret << std::endl;
            write(1, buf, ret);
        } 
    }
    std::cout << "END" << std::endl;
}

/*
std::pair<std::size_t, std::size_t>    Static_response::fill_buf(char *buf_dst, char *buf_src, std::size_t *bytes_count, std::pair<std::size_t, std::size_t> offsets)
{  
    std::cout << "FILL_BUFF" << std::endl;
    std::size_t save_bytes_count = *bytes_count;
    if (save_bytes_count > 4096 || save_bytes_count < offsets.second)
        save_bytes_count = 4096;
    while (offsets.first != save_bytes_count)
    {
        if (offsets.first > offsets.second)
        {
            std::cout << "OKAY0" << std::endl;
            std::cout << "offsets.first: " << offsets.first << std::endl;
            std::cout << "offsets.second: " << offsets.second << std::endl;
            std::cout << "save_bytes_count: " << save_bytes_count << std::endl;
            std::memcpy(buf_dst + offsets.first, buf_src + offsets.second, save_bytes_count - offsets.first);
            
            std::cout << "*bytes_count0: " << *bytes_count << std::endl;
            std::cout << "save_bytes_count: " << save_bytes_count << std::endl;
            *bytes_count -= save_bytes_count - offsets.first;
            //offsets.first += save_bytes_count - offsets.first;
            offsets.second += save_bytes_count - offsets.first;
            //offsets.first += save_bytes_count - offsets.first;
            offsets.first = 0;
             // if offsets.second
            std::cout << "offsets.second: " << offsets.second << std::endl;
            std::cout.write(buf_dst, 4096) << "END" << std::endl;
            return offsets;
            std::cout << "*bytes_count1: " << *bytes_count << std::endl;
            //std::cout << "offsets.first" << offsets.first << std::endl;
        }
            
        else
        {
            std::cout << "OKAY1" << std::endl;
            std::cout << "offsets.first: " << offsets.first << std::endl;
            std::cout << "offsets.second: " << offsets.second << std::endl;
            std::cout << "save_bytes_count: " << save_bytes_count << std::endl;
            std::memcpy(buf_dst + offsets.first, buf_src + offsets.second, save_bytes_count - offsets.second);
            std::cout.write(buf_dst, save_bytes_count - offsets.second) << "END667" << std::endl;
            offsets.first += save_bytes_count - offsets.second;
            offsets.second = 0;
            save_bytes_count -= offsets.second;
            *bytes_count -= save_bytes_count - offsets.second;
        }

    }
    std::cout.write(buf_dst, 4096) << "END" << std::endl;
    std::cout << "*bytes_count: " << *bytes_count << std::endl;
    return  offsets;
    //exit(0);
}
*/

std::pair<std::size_t, std::size_t>    Static_response::fill_buf_header(char *buf_dst, char *buf_src, std::size_t *bytes_count, std::pair<std::size_t, std::size_t> offsets)
{  
    std::cout << "FILL_BUFF" << std::endl;
    std::size_t save_bytes_count = *bytes_count + 2;
    std::cout << "save_bytes_count avant: " << save_bytes_count << std::endl;
    //if (save_bytes_count > 4096 || offsets.second > save_bytes_count)
    if (save_bytes_count > 4096)
        save_bytes_count = 4096;
    std::cout << "offsets.first0: " << offsets.first << std::endl;
    std::cout << "offsets.second0: " << offsets.second << std::endl;
    std::cout << "save_bytes_count0: " << save_bytes_count << std::endl;
    

    std::cout.write(buf_src, save_bytes_count) << "END" << std::endl;
    //std::cout << "(buf_src + offsets.second)[save_bytes_count]: " << (buf_src + offsets.second)[save_bytes_count] << std::endl;
    

    std::cout << "EXIT667" << std::endl;
    //exit(0);
    if (offsets.first > offsets.second)
    {
        std::cout << "OKAY0" << std::endl;

        if (offsets.first > save_bytes_count && save_bytes_count > 4096 - offsets.first)
            save_bytes_count = 4096;
        if (save_bytes_count - offsets.first < save_bytes_count)
        {
            std::cout << "OKAY1" << std::endl;
            std::memcpy(buf_dst + offsets.first, buf_src + offsets.second, save_bytes_count - offsets.first);
            std::cout.write(buf_dst + offsets.first, save_bytes_count - offsets.first) << "END" << std::endl;
            *bytes_count -= save_bytes_count - offsets.first;
            offsets.second += save_bytes_count - offsets.first;
            offsets.first += save_bytes_count - offsets.first;
        }
            
        else
        {  
            std::cout << "OKAY2" << std::endl;
            std::memcpy(buf_dst + offsets.first, buf_src, save_bytes_count);
            std::cout.write(buf_dst + offsets.first, save_bytes_count) << "END" << std::endl;
            offsets.second += *bytes_count + 1;
            *bytes_count -= save_bytes_count;
            offsets.first += save_bytes_count;
        }
               
        //std::memcpy(buf_dst + offsets.first, buf_src + offsets.second, save_bytes_count - offsets.first);
        //if (save_bytes_count == 4096) // IF IL S'AGIT DU DERNIER MORCEAU ON RENTRE TOUT DIRECTEMENT

        
        //*bytes_count -= save_bytes_count - offsets.first;
        //offsets.first += save_bytes_count - offsets.first;
        //offsets.second += save_bytes_count - offsets.first;
        
        //offsets.first += save_bytes_count - offsets.first;
        //offsets.first = 0;
        // if offsets.second
        std::cout << "offsets.first1: " << offsets.first << std::endl;
        std::cout << "offsets.second1: " << offsets.second << std::endl;
        std::cout << "save_bytes_count1: " << save_bytes_count << std::endl;
        
        //std::cout.write(buf_dst, 4096) << "END" << std::endl;

        //std::cout.write(buf_dst, 4096) << "END" << std::endl;
        //return offsets;

        //std::cout << "offsets.first" << offsets.first << std::endl;
        
        //exit(0);
    }       
    else
    {
        std::cout << "OKAY3" << std::endl;
        std::cout << "save_bytes_count - offsets.second: " << save_bytes_count - offsets.second << std::endl;
        std::cout << "offsets.second - save_bytes_count: " << offsets.second - save_bytes_count << std::endl;
        //std::memcpy(buf_dst + offsets.first, buf_src + offsets.second, save_bytes_count - offsets.second);
        if (offsets.second > save_bytes_count && save_bytes_count > 4096 - offsets.second)
            save_bytes_count = 4096;
        if (4096 - offsets.second < save_bytes_count)
        {
            std::cout << "OKAY4" << std::endl;
            std::memcpy(buf_dst + offsets.first, buf_src + offsets.second, 4096 - offsets.second);
            std::cout.write(buf_dst + offsets.first, 4096 - offsets.second) << "END" << std::endl;
            *bytes_count -= 4096 - offsets.second;
            offsets.first += 4096 - offsets.second;
            offsets.second += 4096 - offsets.second;
            
            
        }
            
        else
        {
            std::cout << "OKAY5" << std::endl;
            std::memcpy(buf_dst + offsets.first, buf_src + offsets.second, save_bytes_count);
            std::cout.write(buf_dst + offsets.first, save_bytes_count) << "END" << std::endl;
            offsets.second += *bytes_count + 1;
            *bytes_count -= save_bytes_count;
            offsets.first += save_bytes_count;
            
        }
            
        //exit(0);
        //std::cout.write(buf_dst, save_bytes_count - offsets.second) << "END667" << std::endl;
        //std::cout.write(buf_dst + offsets.first, save_bytes_count - offsets.first) << "END" << std::endl;
        // FIXER LE BYTES_COUNT
 
        //


        //offsets.first += save_bytes_count - offsets.second;
        //offsets.second += save_bytes_count - offsets.second;
        std::cout << "offsets.first2: " << offsets.first << std::endl;
        std::cout << "offsets.second2: " << offsets.second << std::endl;
        std::cout << "save_bytes_count2: " << save_bytes_count << std::endl;        
        
    }

    
    
    
    return  offsets;
    //exit(0);
}

/*
std::pair<std::size_t, std::size_t>    Static_response::fill_buf_header(char *buf_dst, char *buf_src, std::size_t *bytes_count, std::pair<std::size_t, std::size_t> offsets)
{  
    std::cout << "FILL_BUFF" << std::endl;
    std::size_t save_bytes_count = *bytes_count + 2;
    std::cout << "save_bytes_count avant: " << save_bytes_count << std::endl;
    //if (save_bytes_count > 4096 || offsets.second > save_bytes_count)
    if (save_bytes_count > 4096)
        save_bytes_count = 4096;
    std::cout << "offsets.first0: " << offsets.first << std::endl;
    std::cout << "offsets.second0: " << offsets.second << std::endl;
    std::cout << "save_bytes_count0: " << save_bytes_count << std::endl;
    

    std::cout.write(buf_src, save_bytes_count) << "END" << std::endl;
    //std::cout << "(buf_src + offsets.second)[save_bytes_count]: " << (buf_src + offsets.second)[save_bytes_count] << std::endl;
    

    std::cout << "EXIT667" << std::endl;
    //exit(0);
    if (offsets.first > offsets.second)
    {
        std::cout << "OKAY0" << std::endl;

        if (4096 - offsets.first < save_bytes_count)
        {
            std::cout << "OKAY1" << std::endl;
            std::memcpy(buf_dst + offsets.first, buf_src + offsets.second, 4096 - offsets.first);
            std::cout.write(buf_dst + offsets.first, 4096 - offsets.first) << "END" << std::endl;
            *bytes_count -= 4096 - offsets.first;
            offsets.second += 4096 - offsets.first;
            offsets.first += 4096 - offsets.first;
        }
            
        else
        {  
            std::cout << "OKAY2" << std::endl;
            std::memcpy(buf_dst + offsets.first, buf_src, save_bytes_count);
            std::cout.write(buf_dst + offsets.first, save_bytes_count) << "END" << std::endl;
            offsets.second += *bytes_count + 1;
            *bytes_count -= save_bytes_count;
            offsets.first += save_bytes_count;
        }
               
        //std::memcpy(buf_dst + offsets.first, buf_src + offsets.second, save_bytes_count - offsets.first);
        //if (save_bytes_count == 4096) // IF IL S'AGIT DU DERNIER MORCEAU ON RENTRE TOUT DIRECTEMENT

        
        // *bytes_count -= save_bytes_count - offsets.first;
        //offsets.first += save_bytes_count - offsets.first;
        //offsets.second += save_bytes_count - offsets.first;
        
        //offsets.first += save_bytes_count - offsets.first;
        //offsets.first = 0;
        // if offsets.second
        std::cout << "offsets.first1: " << offsets.first << std::endl;
        std::cout << "offsets.second1: " << offsets.second << std::endl;
        std::cout << "save_bytes_count1: " << save_bytes_count << std::endl;
        
        //std::cout.write(buf_dst, 4096) << "END" << std::endl;

        //std::cout.write(buf_dst, 4096) << "END" << std::endl;
        //return offsets;

        //std::cout << "offsets.first" << offsets.first << std::endl;
        
        //exit(0);
    }       
    else
    {
        std::cout << "OKAY3" << std::endl;
        std::cout << "save_bytes_count - offsets.second: " << save_bytes_count - offsets.second << std::endl;
        std::cout << "offsets.second - save_bytes_count: " << offsets.second - save_bytes_count << std::endl;
        //std::memcpy(buf_dst + offsets.first, buf_src + offsets.second, save_bytes_count - offsets.second);
        if (4096 - offsets.second < save_bytes_count)
        {
            std::cout << "OKAY4" << std::endl;
            std::memcpy(buf_dst + offsets.first, buf_src + offsets.second, 4096 - offsets.second);
            std::cout.write(buf_dst + offsets.first, 4096 - offsets.second) << "END" << std::endl;
            *bytes_count -= 4096 - offsets.second;
            offsets.first += 4096 - offsets.second;
            offsets.second += 4096 - offsets.second;
            
            
        }
            
        else
        {
            std::cout << "OKAY5" << std::endl;
            std::memcpy(buf_dst + offsets.first, buf_src + offsets.second, save_bytes_count);
            std::cout.write(buf_dst + offsets.first, save_bytes_count) << "END" << std::endl;
            offsets.second += *bytes_count + 1;
            *bytes_count -= save_bytes_count;
            offsets.first += save_bytes_count;
            
        }
            
        //exit(0);
        //std::cout.write(buf_dst, save_bytes_count - offsets.second) << "END667" << std::endl;
        //std::cout.write(buf_dst + offsets.first, save_bytes_count - offsets.first) << "END" << std::endl;
        // FIXER LE BYTES_COUNT
 
        //


        //offsets.first += save_bytes_count - offsets.second;
        //offsets.second += save_bytes_count - offsets.second;
        std::cout << "offsets.first2: " << offsets.first << std::endl;
        std::cout << "offsets.second2: " << offsets.second << std::endl;
        std::cout << "save_bytes_count2: " << save_bytes_count << std::endl;        
        
    }

    
    
    
    return  offsets;
    //exit(0);
}
*/

std::pair<std::size_t, std::size_t>    Static_response::fill_buf_body(char *buf_dst, char *buf_src, std::size_t *bytes_count, std::pair<std::size_t, std::size_t> offsets)
{  
    std::cout << "FILL_BUFF" << std::endl;
    std::size_t save_bytes_count = *bytes_count;
    if (save_bytes_count > 4096)
        save_bytes_count = 4096;
    std::cout << "offsets.first0: " << offsets.first << std::endl;
    std::cout << "offsets.second0: " << offsets.second << std::endl;
    std::cout << "save_bytes_count0: " << save_bytes_count << std::endl;
    

    
    if (offsets.first > offsets.second)
    {
        std::cout << "OKAY0" << std::endl;
        /*
        if (save_bytes_count < offsets.first)
        {
            std::cout << "OKAY1" << std::endl;
            std::memcpy(buf_dst + offsets.first, buf_src + offsets.second, save_bytes_count);

            std::cout.write(buf_dst + offsets.first, save_bytes_count) << "END" << std::endl; //81
            //std::cout.write(buf_dst, 57) << "END" << std::endl;
            *bytes_count -= save_bytes_count;
        }  
        else
        {
            std::cout << "OKAY2" << std::endl;
            std::memcpy(buf_dst + offsets.first, buf_src + offsets.second, save_bytes_count - offsets.first);
            std::cout.write(buf_dst + offsets.first, save_bytes_count - offsets.first) << "END" << std::endl;
            *bytes_count -= save_bytes_count - offsets.first;
        }
        */
        //if (offsets.first > save_bytes_count)
        //    save_bytes_count = 4096;
        if (offsets.first > save_bytes_count && save_bytes_count > 4096 - offsets.first)
            save_bytes_count = 4096;
        if (save_bytes_count - offsets.first < save_bytes_count)
        {
            std::cout << "OKAY1" << std::endl;
            std::memcpy(buf_dst + offsets.first, buf_src + offsets.second, save_bytes_count - offsets.first);
            std::cout.write(buf_dst + offsets.first, save_bytes_count - offsets.first) << "END" << std::endl;
            *bytes_count -= save_bytes_count - offsets.first;
        }
            
        else
        {  
            std::cout << "OKAY2" << std::endl;
            std::memcpy(buf_dst + offsets.first, buf_src + offsets.second, save_bytes_count);
            std::cout.write(buf_dst + offsets.first, save_bytes_count) << "END" << std::endl;
            *bytes_count -= save_bytes_count;
        }
               
        //std::memcpy(buf_dst + offsets.first, buf_src + offsets.second, save_bytes_count - offsets.first);
        //if (save_bytes_count == 4096) // IF IL S'AGIT DU DERNIER MORCEAU ON RENTRE TOUT DIRECTEMENT
        /*
        if (save_bytes_count > 4096 - offsets.first)
        {
            std::cout << "OKAY1" << std::endl;
            std::memcpy(buf_dst + offsets.first, buf_src + offsets.second, save_bytes_count - offsets.first);
            //std::cout.write(buf_src + offsets.second, save_bytes_count - offsets.first) << "END667" << std::endl;
        }
        else
        {
            std::cout << "OKAY2" << std::endl;
            // SAVE_BYTES_COUNT DOIT ETRE EGALE A 1956 // PEUT FAIRE MEME PROCEDURE POUR LE ELSE
            std::memcpy(buf_dst + offsets.first, buf_src + offsets.second, save_bytes_count - offsets.);
            //std::cout.write(buf_src + offsets.second, save_bytes_count) << "END667" << std::endl; // save_bytes_count
            exit(0);
        }
        */
        
        //*bytes_count -= save_bytes_count - offsets.first;
        //offsets.first += save_bytes_count - offsets.first;
        offsets.second += save_bytes_count - offsets.first;
        offsets.first += save_bytes_count - offsets.first;
        //offsets.first = 0;
        // if offsets.second
        std::cout << "offsets.first1: " << offsets.first << std::endl;
        std::cout << "offsets.second1: " << offsets.second << std::endl;
        std::cout << "save_bytes_count1: " << save_bytes_count << std::endl;
        
        //std::cout.write(buf_dst, 4096) << "END" << std::endl;

        //std::cout.write(buf_dst, 4096) << "END" << std::endl;
        //return offsets;

        //std::cout << "offsets.first" << offsets.first << std::endl;
        
        //exit(0);
    }       
    else
    {
        std::cout << "OKAY3" << std::endl;
        std::cout << "save_bytes_count - offsets.second: " << save_bytes_count - offsets.second << std::endl;
        std::cout << "offsets.second - save_bytes_count: " << offsets.second - save_bytes_count << std::endl;
        //std::memcpy(buf_dst + offsets.first, buf_src + offsets.second, save_bytes_count - offsets.second);
        if (offsets.second > save_bytes_count && save_bytes_count > 4096 - offsets.second)
            save_bytes_count = 4096;
        if (save_bytes_count > offsets.second && save_bytes_count - offsets.second < save_bytes_count)
        {
            std::cout << "OKAY4" << std::endl;
            std::memcpy(buf_dst + offsets.first, buf_src + offsets.second, save_bytes_count - offsets.second);
            std::cout.write(buf_dst + offsets.first, save_bytes_count - offsets.second) << "END" << std::endl;
            *bytes_count -= save_bytes_count - offsets.second;

        }
            
        else
        {
            std::cout << "OKAY5" << std::endl;
            std::memcpy(buf_dst + offsets.first, buf_src + offsets.second, save_bytes_count);
            std::cout.write(buf_dst + offsets.first, save_bytes_count) << "END" << std::endl;
            *bytes_count -= save_bytes_count;
        }
            
        //exit(0);
        //std::cout.write(buf_dst, save_bytes_count - offsets.second) << "END667" << std::endl;
        //std::cout.write(buf_dst + offsets.first, save_bytes_count - offsets.first) << "END" << std::endl;
        // FIXER LE BYTES_COUNT
        /*
        *bytes_count -= save_bytes_count - offsets.second;
        offsets.first += save_bytes_count - offsets.second;
        offsets.second = 0;
        save_bytes_count -= offsets.second;
        */
        //

        /* SAVE
        offsets.second += save_bytes_count - offsets.first;
        offsets.first += save_bytes_count - offsets.first;
        */
        offsets.first += save_bytes_count - offsets.second;
        offsets.second += save_bytes_count - offsets.second;
        std::cout << "offsets.first2: " << offsets.first << std::endl;
        std::cout << "offsets.second2: " << offsets.second << std::endl;
        std::cout << "save_bytes_count2: " << save_bytes_count << std::endl;        
        //exit(0);
    }

    
    
    
    return  offsets;
    //exit(0);
}

std::size_t    Static_response::add_cgi_length(Response_buffers *resp_buf, Response_buffers *rsp_buf)
{
    std::stringstream   ss;
    std::string content_length("Content-Length: ");
    std::size_t offset = resp_buf->getHeaderEnd();
    std::size_t bytes_count;
    std::size_t i = resp_buf->getIndex();
    FILE    *tmpf;
    int ret;

    ss << rsp_buf->getCountBytes() - rsp_buf->getHeaderEnd() - 2;
    content_length += ss.str();
    content_length += "\r\n\r\n";
    bytes_count = content_length.length();
    while (bytes_count && i < 8)
    {
        if (bytes_count < 4096 - offset)
        {
            std::memcpy((*resp_buf->getBuffersSetPtr())[i].first + offset, content_length.c_str(), bytes_count);
            bytes_count = 0;
        }   
        else
        {
            std::memcpy((*resp_buf->getBuffersSetPtr())[i].first + offset, content_length.c_str(), 4096 - offset);
            bytes_count -= 4096 - offset;
            i++;
        }
        
    }
    if (bytes_count)
    {
        i++;
        tmpf = std::tmpfile();
        write(fileno(tmpf), content_length.c_str(), bytes_count);
    }
    resp_buf->setIndex(i);
    resp_buf->setTmpFile(tmpf);
    return content_length.length();
}

void    Static_response::fill_tmpfile(std::pair<Response_buffers*, Response_buffers*> buffs, std::size_t bytes_count, std::size_t index, std::size_t offset)
{
    
    FILE    *tmpf1;
    FILE    *tmpf2;
    char    buf[4096];
    int     ret = 1;

    std::cout << "BYTEST_COUNT0: " << bytes_count << std::endl;
    std::cout << "buffs.first->getIndex(): " << buffs.first->getIndex() << std::endl;
    if (buffs.first->getIndex() == 8)
        buffs.first->setTmpFile(std::tmpfile());
    tmpf1 = buffs.first->getTmpFile();
    for (std::size_t    i = index; i < 8; i++)
    {
        std::cout << "OKAY0" << std::endl;
        if (bytes_count > 4096 - offset)
        {
            std::cout << "OKAY1" << std::endl;
            std::memcpy(buf, (*buffs.second->getBuffersSetPtr())[i].first + offset, 4096 - offset);
            write(fileno(tmpf1), buf, 4096 - offset);
            bytes_count -= 4096 - offset;
            offset = 0;
        }
        else
        {
            std::cout << "OKAY2" << std::endl;
            std::memcpy(buf, (*buffs.second->getBuffersSetPtr())[i].first + offset, bytes_count);
            write(fileno(tmpf1), buf, bytes_count);
            bytes_count = 0;
        }
    }
    std::cout << "BYTEST_COUNT1: " << bytes_count << std::endl;
    if (bytes_count)
    {
        std::cout << "OKAY3" << std::endl;
        tmpf2 = buffs.second->getTmpFile();
        while (ret)
        {
            ret = read(fileno(tmpf2), buf, 4096);
            std::cout << "RET667: " << ret << std::endl;
            write(fileno(tmpf1), buf, ret);
        }
        std::rewind(tmpf1);
    }
    //exit(0);
}

void    Static_response::add_cgi_body(std::size_t bytes_count, std::size_t offset_start, Response_buffers *resp_buf, Response_buffers *rsp_buf)
{
    std::cout << "ADD_CGI_BODY" << std::endl;
    std::pair<std::size_t, std::size_t> offsets(offset_start, rsp_buf->getHeaderEnd() + rsp_buf->getHeaderEndType());
    std::size_t i = resp_buf->getIndex();
    std::size_t j = 0;
    FILE    *tmpf;

    while (bytes_count > 0 && i < 8) // && tmpfile
    {
        std::cout << "offsets.first: " << offsets.first << " et offsets.second: " << offsets.second << std::endl;
        std::cout << "resp_buf->getBuffersSetPtr()->size(): " << resp_buf->getBuffersSetPtr()->size() << std::endl;
        std::cout << "offset20: " << offsets.second << std::endl;
        std::cout << "bytes_coun1: " << bytes_count << std::endl;
        std::cout << "i: " << i << " et j: " << j << std::endl;
        offsets = fill_buf_body((*resp_buf->getBuffersSetPtr())[i].first, (*rsp_buf->getBuffersSetPtr())[j].first, &bytes_count, offsets);
        if (offsets.second == 4096)
        {
            offsets.second = 0;
            j++;
        }
        if (offsets.first == 4096)
        {
            offsets.first = 0;
            i++;
            resp_buf->add_buffer();
           
        }
    }
    

    //exit(0);  
    // AJOUTET FICHIER TEMPORAIRE 
    std::cout << "BYTES_COUNT974: " << bytes_count << std::endl;
    std::cout << "i: " << i << " j: " << j << std::endl;
    
    if (bytes_count)
        fill_tmpfile(std::pair<Response_buffers*, Response_buffers*>(resp_buf, rsp_buf), bytes_count, j, offsets.second);   
}

void    Static_response::prepare_cgi_response(Response_buffers *rsp_buf)
{
    //std::cout << "PREPARE_CGI_RESPONSE: "  << std::endl;
    //std::cout << "rsp_buf->getHeaderEnd(): " << rsp_buf->getHeaderEnd() << std::endl;
    
    std::size_t offset;
    std::stringstream   ss;
    std::size_t length;
    // FAIRE UNE FONCTION
    //add_cgi_request_line()
    std::string response_line("HTTP/1.1");
    response_line += " ";
    response_line += "200";
    response_line += " ";
    response_line += "OK";
    response_line += "\r\n";
    //
    Response_buffers    *resp_buf = _rsp_buf_tree_ptr->find(_fd)->second;
    
    std::cout << "INDEX0: " << resp_buf->getIndex() << std::endl;
    std::vector<std::pair<char *, std::size_t>> *buf_set = resp_buf->getBuffersSetPtr();
    resp_buf->add_buffer();
    std::memcpy((*buf_set)[0].first, response_line.c_str(), response_line.length());
    resp_buf->setHeaderEnd(rsp_buf->getHeaderEnd());

    std::size_t save_header_end = resp_buf->getHeaderEnd();
    add_cgi_header(resp_buf->getHeaderEnd(), 17, resp_buf, rsp_buf);

    //print_response_header(resp_buf->getBuffersSetPtr(), rsp_buf->getTmpFile(), resp_buf->getHeaderEnd());

  
    length = add_cgi_length(resp_buf, rsp_buf);

    add_cgi_body(rsp_buf->getCountBytes() - rsp_buf->getHeaderEnd() - 2, resp_buf->getHeaderEnd() + length, resp_buf, rsp_buf);

    //print_response_header(resp_buf->getBuffersSetPtr(), resp_buf->getTmpFile(), resp_buf->getHeaderEnd() + length + rsp_buf->getCountBytes() - rsp_buf->getHeaderEnd() - 2);

    resp_buf->setCountBytes(resp_buf->getHeaderEnd() + length + rsp_buf->getCountBytes() - rsp_buf->getHeaderEnd() - 2);

    return ;
}

void    Static_response::setRequestParser(Request_parser req_pars)
{
    _req_pars = req_pars;
}

void    Static_response::send_response()
{
    std::cout << "\nSEND RESPONSE" << std::endl;
    std::map<int, Response_buffers *>::iterator it = getBufTreePtr()->find(_fd);
    std::size_t i = 0;
    std::size_t bytes_count = it->second->getCountBytes();
    char    buf[4096];
    //
    std::size_t ret;
    // EPOLL
    std::cout << "it->second->getIndex(): " << it->second->getIndex() << std::endl;
    std::cout << "bytes_count: " << bytes_count << std::endl;
    std::cout << "i: " << i << std::endl;
    std::cout << "OKAY0" << std::endl;
    //std::cout << "EXIT667" << std::endl;
    //exit(0);
    for (; i <= it->second->getIndex() && i < 8 && bytes_count; i++)
    {
        std::cout << "OKAY1" << std::endl;
        if (bytes_count > 4096)
        {
            std::cout << "OKAY2" << std::endl;
            ret = send(_fd, (*it->second->getBuffersSetPtr())[i].first, 4096, 0);
            write(1, (*it->second->getBuffersSetPtr())[i].first, 4096);
            std::cout << "RET0: " << ret << std::endl;
            bytes_count -= 4096;
        } 
        else
        {
            std::cout << "OKAY3" << std::endl;
            ret = send(_fd, (*it->second->getBuffersSetPtr())[i].first, bytes_count, 0);
            write(1, (*it->second->getBuffersSetPtr())[i].first, bytes_count);
            bytes_count = 0;
            std::cout << "RET1: " << ret << std::endl;
        }
    }
    std::cout << "OKAY4" << std::endl;
    std::cout << "BYTES_COUNT: " << bytes_count << std::endl;
    //std::cout << "EXIT667" << std::endl;
    //exit(0);
    if (bytes_count)
    {
        std::cout << "OKAY5" << std::endl;
        while (ret)
        {
            if (bytes_count > 4096)
            {
                ret = read(fileno(it->second->getTmpFile()), buf, 4096);
                send(_fd, buf, 4096, 0);
                bytes_count -= 4096;
            }
            else
            {
                ret = read(fileno(it->second->getTmpFile()), buf, bytes_count);
                send(_fd, buf, bytes_count, 0);
                bytes_count = 0;
            }
        }
         
    }
    std::cout << "OKAY6" << std::endl;
    //reset response_handler
    it->second->setIndex(0);
    it->second->setCountBytes(0);
    it->second->getBuffersSetPtr()->clear();
}

void    Static_response::error_response(int code)
{
    std::string status_code;
    std::string reason_phrase;

    if (code == 400)
    {
        status_code = "400";
        reason_phrase = "Bad Request";        
    }
    if (code == 403)
    {
        status_code = "403";
        reason_phrase = "Forbidden";        
    }
    if (code == 404)
    {
        status_code = "404";
        reason_phrase = "Not Found";
    }
    if (code == 405)
    {
        status_code = "405";
        reason_phrase = "Method Not Allowed";        
    }
    if (code == 500)
    {
        status_code = "500";
        reason_phrase = "Internal Server Error";         
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
    body += "</h1></center>\r\n<hr><center>webserv/1.0 (Ubuntu)</center>\r\n</body>\r\n</html>\r\n";
    std::string content_length("Content-Length: ");
    std::stringstream   str_convert;
    str_convert << body.length();
    content_length += str_convert.str();
    content_length += "\r\n\r\n";
    Response_buffers *resp_buf = _rsp_buf_tree_ptr->find(_fd)->second;
    resp_buf->add_buffer();
    std::memcpy((*resp_buf->getBuffersSetPtr())[0].first, response_line.c_str(), response_line.length());
    std::memcpy((*resp_buf->getBuffersSetPtr())[0].first + response_line.length(), content_length.c_str(), content_length.length());
    std::memcpy((*resp_buf->getBuffersSetPtr())[0].first + response_line.length() + content_length.length(), body.c_str(), body.length());
    resp_buf->setCountBytes(response_line.length() + content_length.length() + body.length());
    //std::cout.write((*resp_buf->getBuffersSetPtr())[0].first, resp_buf->getCountBytes()) << "END" << std::endl;
}

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

std::map<int, Response_buffers*> Static_response::getBufTree() const
{
    return _rsp_buf_tree;
}

std::map<int, Response_buffers*> *Static_response::getBufTreePtr() const
{
    return _rsp_buf_tree_ptr;
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


