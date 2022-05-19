#include "include/ServerSetup.hpp"

// --------------------------------------------------------- //
// --------------- Constructors and Operators -------------- //
// --------------------------------------------------------- //
ServerSetup::ServerSetup()
{
    this->listen = std::pair<short, u_int32_t> (); 
    this->server_name = std::vector<std::string>();
    this->root = std::string();
    this->index = std::vector<std::string>();
    this->error_pages = std::vector<std::pair<short, std::string> >();
    this->client_max_body_size = int();
    this->request_method =  std::vector<std::string>();
    this->autoindex = std::string();
    this->locations = std::vector<t_location>();
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
    return (*this);
}



// --------------------------------------------------------- //
// ----------------- Member Methods ------------------------ //
// --------------------------------------------------------- //
t_location ServerSetup::initLocation()
{
    t_location location;
    location.path = std::string();
    location.root = std::string();
    location.index = std::vector<std::string>();
    location.error_pages = std::vector<std::pair<short, std::string> >();
    location.client_max_body_size = int();
    location.request_method =  std::vector<std::string>();
    location.autoindex = std::string();

    return (location);
}
