#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>

// #define PORT 80

int main(int argc, char **argv){

  int server_fd, new_socket;
  long valread;
  struct sockaddr_in address;
  int address_len = sizeof(address);

  std::string hello = "Hello from the server";

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    std::cerr << "cannot creat socket" << std::endl; 
    exit(EXIT_FAILURE);
  }

  // memset((char *)&address, 0, (size_t)address_len);
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  address.sin_port = htons(80);

  if (bind(server_fd, (struct sockaddr*)&address, address_len) < 0){
    std::cerr << "bind does not working" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (listen(server_fd, 0) < 0){
    std::cerr << "In listen" << std::endl;
    exit(EXIT_FAILURE);
  }

  while(1){

    // std::cout << "waiting for a new connection" << std::endl; if second par of listen more than 0
    if ((new_socket = accept(server_fd, (struct sockaddr*) &address, (socklen_t*)&address_len)) < 0){
      std::cerr << "In accept" << std::endl;
      exit(EXIT_FAILURE);
    }

    std::string buffer[1024] = {0};
    if ((valread = read(new_socket, buffer, buffer->length())) < 0)
      std::cerr << "No bytes are there to read" << std::endl;

    write(new_socket , hello.c_str() , hello.length());
    std::cout << "Hello msg sent" << std::endl;
    close(new_socket);
  }

  return 0;
}