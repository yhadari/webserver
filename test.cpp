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

  int GetServerFd(){
    return _server_fd;
  }

  int AcceptNewConnection(){

    int new_socket;

    std::cout << "waiting for a new connection" << std::endl;
    if ((new_socket = accept(_server_fd, (struct sockaddr*) &_address, (socklen_t*)&_address_len)) < 0){
      std::cerr << "In accept" << std::endl;
      exit(EXIT_FAILURE);
    }
    return new_socket;

  }
    
  static void handleConnection(int new_socket){

    char response[100] = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
    long valread;
    char request[1024] = {0};
    valread = read(new_socket, request, 1024);
    std::cout << "--------------------------begin-------------------------" <<std::endl;
    std::cout << request << std::endl;
    write(new_socket, response, strlen(response));
    std::cout << "Hello msg sent" << std::endl;
    close(new_socket);

  }

};

int main(int argc, char **argv){

  argc = 0;
  (void)argv;

  //setup server
  Server server1(8080, INADDR_ANY); 
  Server server2(8000, INADDR_ANY);

  fd_set CurrentSockets, ReadySocket;


  //initialize my current set
  FD_ZERO(&CurrentSockets);
  FD_SET(server1.GetServerFd(), &CurrentSockets);
  FD_SET(server2.GetServerFd(), &CurrentSockets);

  while (1){
    //because select desetructive
    ReadySocket = CurrentSockets;

    //FD_SETSIZE: Max FD Socket Can Support
    if (select(FD_SETSIZE, &ReadySocket, NULL, NULL, NULL) < 0){
      std::cerr << "In select" << std::endl;
      exit(EXIT_FAILURE);
    }

    for (int i=0; i < FD_SETSIZE; i++){

      if (FD_ISSET(i, &ReadySocket)){

        if (i == server1.GetServerFd()){
          //this is a new connection
          int client_socket = server1.AcceptNewConnection();
          FD_SET(client_socket, &CurrentSockets);
        }

        else if (i == server2.GetServerFd()){
          //this is a new connection
          int client_socket = server2.AcceptNewConnection();
          FD_SET(client_socket, &CurrentSockets);
        }
        
        else{
          Server::handleConnection(i);
          FD_CLR(i, &CurrentSockets);
        }
      }
    }
  }
  return 0;
}