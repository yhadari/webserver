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
  std::vector<int> v_server_fd;
  std::vector<struct sockaddr_in> v__address;
  int _address_len;
  int opt;

  public:
  Server(std::vector<ServerSetup> servers) : opt(1){

    std::vector<ServerSetup>::iterator it(servers.begin());
    size_t size = servers.size();

    for (size_t i = 0; i < size; i++)
    {
      if ((_server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        std::cerr << "cannot creat socket" << std::endl; 
        exit(EXIT_FAILURE);
      }
      v_server_fd.push_back(_server_fd);

      _address.sin_family = AF_INET; // use IPv4 
      _address.sin_port = htons((*it).listen.first);
      _address.sin_addr.s_addr = (*it).listen.second;
      it++;
      v__address.push_back(_address);

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
  }

  std::vector<int> GetServerFds(){
    return v_server_fd;
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

std::vector<ServerSetup> parse(){

  std::vector<ServerSetup> VServers;
  size_t s = 10;

  ServerSetup servers[s];
  std::pair<short, u_int32_t> p[s];

  for (size_t i = 0; i < s; i++)
  {
    p[i] = std::make_pair(8080+i, INADDR_ANY);
    servers[i].listen = p[i];
    VServers.push_back(servers[i]);
  }

  return VServers;
}

fd_set  set_fds(std::vector<int> server_fds){

  fd_set CurrentSockets;
  size_t size = server_fds.size();
  std::vector<int>::iterator it(server_fds.begin());

  FD_ZERO(&CurrentSockets);
  for (size_t i = 0; i < size; i++)
    FD_SET((*it++), &CurrentSockets);

  return CurrentSockets;
}

int main(int argc, char **argv){

  argc = 0;
  (void)argv;

  std::vector<ServerSetup> servers_setup = parse();

  //setup server
  Server server(servers_setup); 

  //initialize my current set

  fd_set ReadySocket;
  //because select desetructive
  ReadySocket = set_fds(server.GetServerFds());

  while (1){

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