#ifndef __CGI__
#define __CGI__
#include <iostream>

#include "../parser_request/include/RequestInfo.hpp"
#include "../parser_config/include/ServerSetup.hpp"

const std::string                 handle_cgi(std::string path, RequestInfo &request, ServerSetup &server);
std::vector<const char*>    setEnvp(RequestInfo &request, ServerSetup &server);

#endif