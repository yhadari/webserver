#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>

int main(){

  int server_fd, new_socket;
  int bind_ret;
  struct sockaddr_in address;
  socklen_t address_len = sizeof(address);

  memset((char *)&address, 0, address_len);
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  address.sin_port = htons(80);

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    std::cerr << "cannot creat socket" << std::endl; 
    exit(EXIT_FAILURE);
  }

  if ((bind_ret = bind(server_fd, (struct sockaddr*) &address, address_len)) < 0){
    std::cerr << "bind does not working" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (listen(server_fd, 0) < 0){
    std::cerr << "In listen" << std::endl;
    exit(EXIT_FAILURE);
  }

  if ((new_socket = accept(server_fd, (struct sockaddr*) &address, &address_len)) << 0){
    std::cerr << "In accept" << std::endl;
    exit(EXIT_FAILURE);
  }
}