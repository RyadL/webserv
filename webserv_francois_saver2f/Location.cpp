#include "includes/webserv.hpp"


Location::Location():_clientBodySize(16000)
{
    std::vector<std::string> methods;
    methods.push_back("GET");
    methods.push_back("POST");
    methods.push_back("DELETE");
    _allowedMethods = methods;
}
Location::Location(const Location & A){*this = A;}
Location::~Location(){}
Location &   Location::operator=(const Location & A)
{
    _title = A._title;
    _prefix = A._prefix;
    _root = A._root;
    _index = A._index;
    _alias = A._alias;
    _errorPages = A._errorPages;
    _clientBodySize = A._clientBodySize;
    _allowedMethods = A._allowedMethods;
    _redirection = A._redirection;
    _autoIndex = A._autoIndex;
    _cgi_pass = A._cgi_pass;
    _cgi_params = A._cgi_params;
    _upload_store = A._upload_store;
    return (*this);
}

//getters


void            Location::parse_error_pages(std::vector<std::string> lineVector)
{
    int             key;
    std::string     value;

    std::vector<int> tab;

    for (int i = 1; i < lineVector.size(); i++)
    {
        if (lineVector[i][0] != '/')
        {
            const char *c = lineVector[i].c_str();
            key = std::atoi(c);
            tab.push_back(key);
        }
        else
            value = lineVector[i];
    }
    for (int i = 0; i < tab.size(); i++)
    {
        std::pair<int, std::string>   p(tab[i], value);
        _errorPages.insert(p);
    }
}

void            Location::parse_client_body_size(std::vector<std::string> lineVector)
{
    const char *c = lineVector[1].c_str();
    _clientBodySize = std::atoi(c) * 1000;
}

void            Location::parse_allow_methods(std::vector<std::string> lineVector)
{
    _allowedMethods.clear();
    for (int i = 1; i < lineVector.size(); i++)
    {
        _allowedMethods.push_back(lineVector[i]);
    }
    _allowedMethods[_allowedMethods.size() - 1].pop_back();
}

void            Location::parse_cgi_param(std::vector<std::string> lineVector)
{
    std::string A(lineVector[1]);
    std::string B(lineVector[2]);
    B.pop_back();
    std::pair<std::string,std::string>   p(A,B);
    _cgi_params.insert(p);
}


unsigned int    Location::parse(std::vector<std::string> fileVector, unsigned int i)
{
    while (!is_closed_chevron(fileVector[i]))
    {
        std::vector<std::string>    lineVector;
        
        lineVector = split(fileVector[i]);

        if (lineVector[0] == "root")
        {
            lineVector[1].pop_back();
            _root = lineVector[1];
        }
        else if (lineVector[0] == "index")
        {
            lineVector[1].pop_back();
            _index = lineVector[1];
        }
        else if (lineVector[0] == "error_page")
            parse_error_pages(lineVector);
        else if (lineVector[0] == "client_body_buffer_size")
            parse_client_body_size(lineVector);
        else if (lineVector[0] == "allow_methods")
            parse_allow_methods(lineVector);
        else if (lineVector[0] == "return")
        {
            lineVector[2].pop_back();
            _redirection = lineVector[2];
        }
        else if (lineVector[0] == "autoindex")
        {
            lineVector[1].pop_back();
            _autoIndex = lineVector[1];
        }
        else if (lineVector[0] == "alias")
        {
            lineVector[1].pop_back();
            _alias = lineVector[1];
        } 
        else if (lineVector[0] == "cgi_pass")
        {
            lineVector[1].pop_back();
            _cgi_pass = lineVector[1];
        }
        else if (lineVector[0] == "cgi_param")
            parse_cgi_param(lineVector); 
        else if (lineVector[0] == "upload_store")
        {
            lineVector[1].pop_back();
            _upload_store = lineVector[1];
        }
        else 
        {
            if (lineVector[0] != "\n")
            {
                //std::cout << "error in the configuration file dodi: " << lineVector[0] << std::endl;
                exit (1);
            }
        }
        i++;
    }
    return i;
}

void    Location::print_location()
{
    /*std::cout << "location "<<_title<<" :\n";
    std::cout << "--------root      : " << _root << std::endl;
    std::cout << "--------index      : " << _index << std::endl;*/
    
    
    for (std::map<int,std::string>::iterator it = _errorPages.begin(); it != _errorPages.end(); it++)
    {
        /*std::cout << "--------error pages : number : " << it->first << std::endl;
        std::cout << "--------error_page : uri :"  << it->second << std::endl;*/
    }
    
    //std::cout << "--------client_body_size : "  << _clientBodySize << std::endl;

    for (int i = 0; i < _allowedMethods.size(); i++)
        std::cout << "--------allow_methods : "  << _allowedMethods[i] << std::endl;

    /*std::cout << "--------redirection : "  << _redirection << std::endl;
    std::cout << "--------autoindex : "  << _autoIndex << std::endl;
    std::cout << "--------alias : "  << _alias << std::endl;
    std::cout << "--------cgi_pass : "  << _cgi_pass << std::endl;*/

    for (std::map<std::string,std::string>::iterator it = _cgi_params.begin(); it != _cgi_params.end(); it++)
    {
        /*std::cout << "--------cgi_param : variable : " << it->first << std::endl;
        std::cout << "--------cgi_param : param :"  << it->second << std::endl;*/
    }

    /*std::cout << "--------upload_store : "  << _upload_store << std::endl;
    std::cout << "----------------fin----------------\n\n";*/
}

std::string Location::getCgiPass() const
{
    return _cgi_pass;
}

std::string Location::getRootDirectory() const
{
    return _root;
}

std::string Location::getIndex() const
{
    return _index;
}

std::vector<std::string> Location::getAllowedMethods() const
{
    return _allowedMethods;
}