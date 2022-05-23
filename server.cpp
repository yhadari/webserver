#include "include/Server.hpp"

Server::Server(std::vector<ServerSetup> servers) : _address_len(sizeof(_address)), opt(1){

    std::vector<ServerSetup>::iterator it(servers.begin());
    size_t size = servers.size();

    for (size_t i = 0; i < size; i++)
    {
      if ((_server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        std::cerr << "cannot creat socket" << std::endl; 
        exit(EXIT_FAILURE);
      }
      _v_server_fd.push_back(_server_fd);

      memset(&_address, 0, _address_len);
      _address.sin_family = AF_INET; // use IPv4 
      _address.sin_port = htons((*it).getListen().first);
      _address.sin_addr.s_addr = (*it).getListen().second;
      it++;
      _v_address.push_back(_address);

      //Control socket descriptors
      if (fcntl(_server_fd, F_SETFL, O_NONBLOCK) < 0){
        std::cerr << "non_blocking error" << std::endl;
        exit(EXIT_FAILURE);
      }

      if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0){
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

  std::vector<int> Server::GetServerFds(){
    return _v_server_fd;
  }

  //pair<server, possition>
  int Server::AcceptNewConnection(std::pair<int, size_t> pair){

    int new_socket;

    if ((new_socket = accept(pair.first, (struct sockaddr*) &_v_address[pair.second], (socklen_t*)&_address_len)) < 0){
      std::cerr << "In accept" << std::endl;
      exit(EXIT_FAILURE);
    }
    return new_socket;

  }
    
   void Server::handleConnection(int new_socket){

    char response[100] = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
    long valread;
    char request[1024] = {0};
    valread = recv(new_socket, request, 1024, 0);
    std::cout << "--------------------------begin-------------------------" <<std::endl;
    std::cout << request << std::endl;
    send(new_socket, response, strlen(response), 0);
    close(new_socket);
  }