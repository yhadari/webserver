#ifndef __UTILS_HPP__
#define __UTILS_HPP__

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
#include <fstream>
#include <sys/stat.h> // stat of file
#include "EnumRequestTarget.hpp"

void                                        set_fds(fd_set &SocketsRead, fd_set &SocketsWrite, std::vector<int> server_fds);
std::pair<bool, std::pair<int, size_t> >    find_fd(int fd, std::vector<int> server_fds);
bool                                        samePort(std::vector<struct sockaddr_in> v_address);

int                                         stringToInt(const std::string s);
bool                                        isNumber(const std::string& s);

std::string                                 fileToSring(const char *file);
std::vector<std::string>                    split(std::string s, std::string del);
long                                        sizeFile(std::string path);

TypeRequestTarget                           getPathType(const std::string& uri);
int                                         coutChar(const std::string &s, char del);
unsigned int                                hexToDecimal(const std::string &s);

#endif