#include "../include/Utils.hpp"

void set_fds(fd_set &CurrentSockets, std::vector<int> server_fds){

  size_t size = server_fds.size();
  std::vector<int>::iterator it(server_fds.begin());

  FD_ZERO(&CurrentSockets);
  for (size_t i = 0; i < size; i++)
    FD_SET((*it++), &CurrentSockets);
}

//pair<is_find , pair<server_fd, possition>>
std::pair<bool, std::pair<int, size_t> >  find_fd(int fd, std::vector<int> server_fds){
    size_t size = server_fds.size();
    std::vector<int>::iterator it(server_fds.begin());

    for (size_t i = 0; i < size; i++)
    {
      if (fd == *it)
        return  std::make_pair(true, std::make_pair(*it, i));
      it++;
    }
    return std::make_pair(false, std::make_pair(0, 0));
}