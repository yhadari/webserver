#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <stdlib.h>
#include <unistd.h>

class Server{

  private:
  int _server_fd;
  struct sockaddr_in _address;
  int _address_len;

  public:
  Server(uint16_t port, uint32_t addr) : _address_len(sizeof(_address)){
    if ((_server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
      std::cerr << "cannot creat socket" << std::endl; 
      exit(EXIT_FAILURE);
    }

    _address.sin_family = AF_INET;
    _address.sin_addr.s_addr = addr;
    _address.sin_port = htons(port);

    if (bind(_server_fd, (struct sockaddr*)&_address, _address_len) < 0){
      std::cerr << "bind does not working" << std::endl;
      exit(EXIT_FAILURE);
    }

    if (listen(_server_fd, 0) < 0){
      std::cerr << "In listen" << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  void Start(){

    char response[100] = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
    int new_socket;
    long valread;

    while(1){

      std::cout << "waiting for a new connection" << std::endl;
      if ((new_socket = accept(_server_fd, (struct sockaddr*) &_address, (socklen_t*)&_address_len)) < 0){
        std::cerr << "In accept" << std::endl;
        exit(EXIT_FAILURE);
      }

      char request[1024] = {0};
      valread = read(new_socket, request, 1024);
      std::cout << "--------------------------begin-------------------------" <<std::endl;
      std::cout << request << std::endl;

      write(new_socket, response, strlen(response));
      std::cout << "Hello msg sent" << std::endl;
      close(new_socket);
    }
  }
};

int main(int argc, char **argv){

  argc = 0;
  (void)argv;

  Server server1(8080, INADDR_ANY);
  server1.Start();
  return 0;
}