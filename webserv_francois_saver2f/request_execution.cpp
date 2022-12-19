#include "includes/webserv.hpp"


// NE PAS OUBLIER DE PASSER LES ARGUMENTS PAR REFERENCE

int    check_allowed_methods_request(std::string method, std::vector<std::string> allowed_methods)
{
    for (std::vector<std::string>::const_iterator   it = allowed_methods.begin(); it != allowed_methods.end(); it++)
    {
        //std::cout << "*it: " << *it << " et method: " << method << std::endl;
        if (*it == method)
        {
            std::cout << "METHOD ALLOWED" << std::endl;
            return 0;
        }
    }
    std::cout << "METHOD NOT ALLOWED" << std::endl;
    return 1;
}

void    handle_request_execution(Request_parser& req_pars, Location& location_chosen, Static_response& rsp_hdl, int fd)
{
    /*
    if (req_pars.getRequestMethod() == "POST" || req_pars.getRequestMethod() == "HEAD")
    {
        rsp_hdl.error_response(667);
        return;
    }
    */
    //std::cout << "OKAY0" << std::endl;

    
    if (check_allowed_methods_request(req_pars.getRequestMethod(), location_chosen.getAllowedMethods()))
    {
        if (req_pars.getRequestMethod() == "POST")
        {   
            rsp_hdl.error_response(667);
        }   
        if (req_pars.getRequestMethod() == "HEAD")
        {   
            rsp_hdl.error_response(666);
        }   
        //rsp_hdl.setAllowedMethod(false);
        return;
    }
    
    req_pars.setRootDirectory(location_chosen.getRootDirectory());
    req_pars.setIndex(location_chosen.getIndex());
    req_pars.setTitle(location_chosen.get_title());
    //std::cout << "req_pars.getTitle()0: " << req_pars.getTitle() << std::endl;
    rsp_hdl.setAllowedMethod(true);
    if (location_chosen.getCgiPass().empty())
    {
        rsp_hdl.setRequestParser(req_pars);
        rsp_hdl.setFd(fd);
        rsp_hdl.execute_request();
    }
                                    
    else
    {
        Cgi_handler cgi_hdl(req_pars);
        cgi_hdl.execute_request();
        rsp_hdl.prepare_cgi_response(cgi_hdl.getBuffer(), cgi_hdl.getLength());
                                    
    }
    
}
