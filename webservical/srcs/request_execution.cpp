#include "../includes/webserv.hpp"

// NE PAS OUBLIER DE PASSER LES ARGUMENTS PAR REFERENCE

int check_allowed_methods_request(std::string method, std::vector<std::string> allowed_methods)
{
    for (std::vector<std::string>::const_iterator it = allowed_methods.begin(); it != allowed_methods.end(); it++)
    {
        std::cout << "*it: " << *it << " et method: " << method << std::endl;
        if (*it == method)
        {
            std::cout << "METHOD ALLOWED" << std::endl;
            return 0;
        }
    }
    std::cout << "METHOD NOT ALLOWED" << std::endl;
    return 1;
}

void handle_request_execution(Request_parser &req_pars, Location &location_chosen, Static_response &rsp_hdl, int fd)
{
    rsp_hdl.setFd(fd);
    //std::cout << "ON EST LA" << std::endl;
    //exit(0);
    
    //rsp_hdl.setFd(fd);
    
    //std::cout.write(*req_pars.getBodyBufferPtr(), req_pars.getBytesInsideBody()) << "END" << std::endl;

    req_pars.setRootDirectory(location_chosen.getRootDirectory());
    req_pars.setIndex(location_chosen.getIndex());
    req_pars.setTitle(location_chosen.get_title());
    rsp_hdl.setAllowedMethod(true);

    rsp_hdl.setRequestParser(req_pars);
    rsp_hdl.upload_file();
    
    if (check_allowed_methods_request(req_pars.getRequestMethod(), location_chosen.getAllowedMethods()))
    {
        rsp_hdl.error_response(405);
        return;
    }

    req_pars.setRootDirectory(location_chosen.getRootDirectory());
    req_pars.setIndex(location_chosen.getIndex());
    req_pars.setTitle(location_chosen.get_title());
    rsp_hdl.setAllowedMethod(true);
    
    // INSERER NOUVEAU CLIENT DANS LA MAP
    std::cout << "location_chosen.getCgiPass(): " << location_chosen.getCgiPass() << std::endl;
    if (location_chosen.getCgiPass().empty())
    {
        std::cout << "OKAY0" << std::endl;
        
        rsp_hdl.setRequestParser(req_pars);
        rsp_hdl.execute_request();
    }

    else
    {
        // 0 QUI EST PASSE EN ARGUMENT SERA REMPLACE PAR UNE VARIABLE QUI INDIQUERA LE SUFFIXE DE SCRIPT CGI QUE L'ON RECHERCHE
        req_pars.parse_request_cgi(1);
        Cgi_handler cgi_hdl(req_pars, rsp_hdl);
        try
        {
            cgi_hdl.execute_request();
        }
        catch (Cgi_handler::CgiScriptNotFound& e)
        {
            rsp_hdl.error_response(404);
            return ;
        }
        catch (Cgi_handler::CgiScriptFailed& e)
        {
            rsp_hdl.error_response(500);
            return ;
        }
            catch (Cgi_handler::CgiScriptInvalid& e)
        {
            rsp_hdl.error_response(500);
            return ;
        }
        std::cout << "cgi_hdl.getResponseBuffersPtr()->getHeaderEndType(): " << cgi_hdl.getResponseBuffersPtr()->getHeaderEndType() << std::endl;

        rsp_hdl.prepare_cgi_response(cgi_hdl.getResponseBuffersPtr());
        std::cout << "FINITO" << std::endl;
        // rsp_hdl.getBufTreePtr()->find(fd)->second->add_buffer();
        //(*rsp_hdl.getBufTreePtr()->find(fd)->second.getBuffersSetPtr())[0]->second = 667;
        // exit(0);
    }
    // exit(0);
}
