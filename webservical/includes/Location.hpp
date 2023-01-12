#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "webserv.hpp"


class   Location
{
    public:
        Location();
        Location(const Location & A);
        ~Location();

        Location &   operator=(const Location & A);

        //getters
        std::string     get_prefix() const {return _prefix;}
        std::string     get_title() const {return _title;}

        //setters
        void    set_prefix(std::string pref){_prefix = pref;}
        void    set_title(std::string title){_title = title;}

        unsigned int    parse(std::vector<std::string> fileVector, unsigned int i);
        void            parse_error_pages(std::vector<std::string>);
        void            parse_client_body_size(std::vector<std::string>);
        void            parse_allow_methods(std::vector<std::string>);
        void            parse_cgi_param(std::vector<std::string>);
        void            print_location();

        std::string getCgiPass() const;
        std::string getRootDirectory() const;
        std::string getIndex() const;
        std::vector<std::string> getAllowedMethods() const;

    private:
        std::string                 _title;
        std::string                 _prefix;
        std::string                 _root;
        std::string                 _index;
        std::string                 _alias;
        std::map<int, std::string>  _errorPages;
        unsigned int                _clientBodySize;
        std::vector<std::string>    _allowedMethods;
        std::string                 _redirection;
        std::string                 _autoIndex;
        std::string                 _cgi_pass;
        std::map<std::string,std::string>   _cgi_params;
        std::string                 _upload_store;

};

#endif