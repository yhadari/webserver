#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "../parser_config/include/Parser.hpp"

#include <iostream>
#include <fstream>
#include <iostream>
#include <sstream> //std::stringstream

void set_fds(fd_set &CurrentSockets, std::vector<int> server_fds);
//pair<is_find , pair<server_fd, possition>>
std::pair<bool, std::pair<int, size_t> >  find_fd(int fd, std::vector<int> server_fds);
bool    samePort(std::vector<struct sockaddr_in> v_address);

int     stringToInt(const std::string s);
bool    isNumber(const std::string& s);

std::string fileToSring(const char *file);