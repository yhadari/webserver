#ifndef SERVER_SETUP_HPP
#define SERVER_SETUP_HPP

#include <iostream>
#include <vector>

typedef struct s_location
{
    std::string                                 path;   // Ex: /root/local/ ,path of location, oblig
    std::string                                 root;
    std::vector<std::string>                    index;
    std::vector<std::pair<short, std::string> > error_pages;
    int                                         client_max_body_size;
    std::vector<std::string>                    request_method;
    std::string                                 autoindex;

} t_location;

class ServerSetup
{
    public:
    //------ public member --------- >>
    std::pair<short, u_int32_t>                 listen;                 // listen                80 0.0.0.0;
    std::vector<std::string>                    server_name;            // server_name           server1 server2;
    std::string                                 root;                   // root                  /var/www/example.com;
    std::vector<std::string>                    index;                  // index                 index.html download.html;
    std::vector<std::pair<short, std::string> > error_pages;            // error_page            404 505 /custom_404.html /custom_505.html ;
    int                                         client_max_body_size;   // client_max_body_size  1024;
    std::vector<std::string>                    request_method;         // request_method        GET POST;
    std::string                                 autoindex;              // autoindex             off;
    t_location                                  location;

    // --------------------------------------------------------- //
    // --------------- Constructors and Operators -------------- //
    // --------------------------------------------------------- //
    ServerSetup();
    ServerSetup(const ServerSetup& server_setup);
    ServerSetup&    operator=(const ServerSetup& server_setup);

    // --------------------------------------------------------- //
    // ----------------- Member Methods ------------------------ //
    // --------------------------------------------------------- //
    void initLocation();

};

#endif