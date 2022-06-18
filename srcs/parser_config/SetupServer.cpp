#include "include/ServerSetup.hpp"
#include "../include/Utils.hpp"
// --------------------------------------------------------- //
// --------------- Constructors and Operators -------------- //
// --------------------------------------------------------- //
ServerSetup::ServerSetup()
{
    this->listen = std::pair<short, u_int32_t> (); listen.first = -1;
    this->server_name = std::vector<std::string>();
    this->root = std::string();
    this->index = std::vector<std::string>();
    this->error_pages = std::vector<std::pair<short, std::string> >();
    this->client_max_body_size = -1;
    this->request_method =  std::vector<std::string>();
    this->autoindex = std::string();
    this->upload_store = std::string();
    this->locations = std::vector<t_location>();
    this->envp = NULL;
    this->_return = std::pair<short, std::string>(); _return.first = -1;
    this->_location_path = std::string();
    this->_php_cgi_path = std::string();
}

ServerSetup::ServerSetup(const ServerSetup& server_setup)
{
    *this = server_setup;
}

ServerSetup&    ServerSetup::operator=(const ServerSetup& server_setup)
{
    this->listen = server_setup.listen;
    this->server_name = server_setup.server_name;
    this->root = server_setup.root;
    this->index = server_setup.index;
    this->error_pages = server_setup.error_pages;
    this->client_max_body_size = server_setup.client_max_body_size;
    this->request_method = server_setup.request_method;
    this->autoindex = server_setup.autoindex;
    this->locations = server_setup.locations;
    this->upload_store  = server_setup.upload_store;
    this->envp = server_setup.envp;
    this->_return = server_setup._return;
    this->_location_path = server_setup._location_path;
    this->_php_cgi_path = server_setup._php_cgi_path;
    return (*this);
}

// --------------------------------------------------------- //
// -------------------- Getters  --------------------------- //
// --------------------------------------------------------- //
std::pair<short, u_int32_t>                 ServerSetup::getListen() const
{
    return (this->listen);
}

std::vector<std::string>                    ServerSetup::getServer_name() const
{
    return (this->server_name);
}

std::string                                 ServerSetup::getRoot() const
{
    return (this->root);
}

std::vector<std::string>                    ServerSetup::getIndex() const
{
    return (this->index);
}

std::vector<std::pair<short, std::string> > ServerSetup::getError_pages() const
{
    return (this->error_pages);
}

int                                         ServerSetup::getClient_max_body_size() const
{
    return (this->client_max_body_size);
}

std::vector<std::string>                    ServerSetup::getRequest_method() const
{
    return (this->request_method);
}  

std::string                                 ServerSetup::getAutoindex() const
{
    return (this->autoindex);
}  

std::vector<t_location>                     ServerSetup::getLocations() const
{
    return (this->locations);
}

std::string                                 ServerSetup::getUploadStore() const
{
    return (this->upload_store);
}   

char**                                      ServerSetup::getEnvp() const
{
    return (*(this->envp));
}

void                                        ServerSetup::setEnvp(char*** envp)
{
    this->envp = envp;
}

std::pair<short, std::string>               ServerSetup::getReturn() const
{
    return (this->_return);
}

std::string                                 ServerSetup::getLocationPath() const
{
    return (this->_location_path);
}

std::string                                 ServerSetup::getPhpCgiPath() const
{
    return (this->_php_cgi_path);
}
// --------------------------------------------------------- //
// -------------------- Member Methods --------------------- //
// --------------------------------------------------------- //
t_location*                                 ServerSetup::isLocation(std::string path) const
{

    for (size_t i = 0; i < getLocations().size(); i++)
        if (getLocations()[i].path == path)
            {
                t_location *location = new t_location();
                *location = getLocations()[i];
                return (location);
            }
    return (NULL);
}

t_location*                                  ServerSetup::getLocation(std::string uri, TypeRequestTarget *type) const
{   
    std::string path = getRoot() + uri;
    t_location* location;
    *type = getPathType(path);

    // Chank the URI directories /../.../..
    path = uri;
    if (this->getAutoindex() == "off")
    {
        while (coutChar(path, '/') > 0) // plus two directory
        {   
            if ((location = isLocation(path)) != NULL)
                return (location);
            path = path.substr(0, path.find_last_of('/'));
        }
    }
    else
    {
        if ((location = isLocation(path)) != NULL)
            return (location);
    }
    return (NULL);
}

// --------------------------------------------------------- //
// ---------------- Non Member Functions ------------------- //
// --------------------------------------------------------- //
t_location ServerSetup::initLocation()
{
    t_location location;
    location.path = std::string();
    location.root = std::string();
    location.index = std::vector<std::string>();
    location.error_pages = std::vector<std::pair<short, std::string> >();
    location.client_max_body_size = -1;
    location.request_method =  std::vector<std::string>();
    location.autoindex = std::string();
    location.upload_store = std::string();
    location._return = std::pair<short, std::string>(); location._return.first = -1;
    location._php_cgi_path = std::string();
    return (location);
}

