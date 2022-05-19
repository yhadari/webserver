#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <stdlib.h>
#include <unistd.h>
#include "../parser_config/include/Parser.hpp"

#include <iostream>
#include <fstream>
#include <iostream>
#include <sstream> //std::stringstream

class Server{

  private:
  int _server_fd;
  struct sockaddr_in _address;
  std::vector<int> _v_server_fd;
  std::vector<struct sockaddr_in> _v_address;
  int _address_len;
  int opt;

  public:
  Server(std::vector<ServerSetup> servers) : _address_len(sizeof(_address)), opt(1){

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
      _address.sin_port = htons((*it).listen.first);
      _address.sin_addr.s_addr = (*it).listen.second;
      it++;
      _v_address.push_back(_address);

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
    return _v_server_fd;
  }

  //pair<server, possition>
  int AcceptNewConnection(std::pair<int, size_t> pair){

    int new_socket;

    std::cout << "waiting for a new connection" << std::endl;
    if ((new_socket = accept(pair.first, (struct sockaddr*) &_v_address[pair.second], (socklen_t*)&_address_len)) < 0){
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

// std::vector<ServerSetup> parser(){

//   std::vector<ServerSetup> VServers;
//   size_t s = 11;

//   ServerSetup servers[s];
//   std::pair<short, u_int32_t> p[s];

//   for (size_t i = 0; i < s; i++)
//   {
//     p[i] = std::make_pair(8080+i, INADDR_ANY);
//     servers[i].listen = p[i];
//     VServers.push_back(servers[i]);
//   }

//   return VServers;
// }

fd_set  set_fds(std::vector<int> server_fds){

  fd_set CurrentSockets;
  size_t size = server_fds.size();
  std::vector<int>::iterator it(server_fds.begin());

  FD_ZERO(&CurrentSockets);
  for (size_t i = 0; i < size; i++)
    FD_SET((*it++), &CurrentSockets);

  return CurrentSockets;
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

std::string fileToSring(const char *file)
{
    std::ifstream inFile;
    inFile.open(file); //open the input file

    std::stringstream strStream;
    strStream << inFile.rdbuf(); //read the file
    std::string str = strStream.str(); //str holds the content of the file

    inFile.close();
    return(str);
}

std::vector<ServerSetup> parseConfig(int argc, char **argv)
{
    std::string contents;
    if (argc == 2)
        contents = fileToSring(argv[1]);
    else
        contents = fileToSring(std::string("test.config").c_str());
     Lexer lexer(contents);
    // Token token(TOKEN_EOF, "\0");
    // while((token = lexer.getNextToken()).type != TOKEN_EOF)
    //     std::cout << "Token \"" << token.type << " | value = \"" << token.value << "\"" << std::endl;

    Parser parser(lexer);
    std::vector<ServerSetup> servers; 
    return (parser.parse());
}

int main(int argc, char **argv){

  argc = 0;
  (void)argv;

  std::vector<ServerSetup> servers_setup = parseConfig(argc, argv);

  //setup server
  Server server(servers_setup); 

  //initialize my current set
  fd_set CurrentSockets, ReadySocket;
  CurrentSockets = set_fds(server.GetServerFds());

  while (1){

    std::pair<bool, std::pair<int, size_t> > search_fd;
    // select desruct all sockets
    ReadySocket = CurrentSockets;

    //FD_SETSIZE: Max FD Socket Can Support
    if (select(FD_SETSIZE, &ReadySocket, NULL, NULL, NULL) < 0){
      std::cerr << "In select" << std::endl;
      exit(EXIT_FAILURE);
    }

    for (int i=0; i < FD_SETSIZE; i++){

      if (FD_ISSET(i, &ReadySocket)){

        if ((search_fd = find_fd(i, server.GetServerFds())).first){
          //this is a new connection
          int client_socket = server.AcceptNewConnection(search_fd.second);
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