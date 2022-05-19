#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <stdlib.h>
#include <unistd.h>
#include "server_setup.hpp"

class Server{

  private:
  int _server_fd;
  struct sockaddr_in _address;
  int _address_len;
  int opt;

  public:
  Server(uint16_t port, uint32_t addr) : _address_len(sizeof(_address)), opt(1){
    if ((_server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
      std::cerr << "cannot creat socket" << std::endl; 
      exit(EXIT_FAILURE);
    }

    _address.sin_family = AF_INET; // use IPv4 
    _address.sin_addr.s_addr = addr;
    _address.sin_port = htons(port);

    if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))){
      std::cerr << "setsockpt error" << std::endl;
      exit(EXIT_FAILURE);
    }

    if (bind(_server_fd, (struct sockaddr*)&_address, _address_len) < 0){
      std::cerr << "bind does not working" << std::endl;
      exit(EXIT_FAILURE);
    }

    //limit backlog is 128
    if (listen(_server_fd, 128) < 0){
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
    valread = recv(new_socket, request, 1024, 0);
    std::cout << "--------------------------begin-------------------------" <<std::endl;
    std::cout << request << std::endl;
    send(new_socket, response, strlen(response), 0);
    close(new_socket);
  }

};

int main(int argc, char **argv){

  argc = 0;
  (void)argv;
  // std::vector<ServerSetup> servers = parse();

  ServerSetup S;

  std::pair<short, u_int32_t> listen(80, INADDR_ANY);
  S.listen = listen;

  //setup server
  Server server1(listen.first, listen.second); 
  Server server2(listen.first, listen.second); 

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