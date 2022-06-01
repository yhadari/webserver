#include "include/Utils.hpp"
#include "include/Server.hpp"


void   run(std::vector<ServerSetup> servers_setup)
{
  std::vector<ServerSetup>::iterator it_b(servers_setup.begin());
  Server server(servers_setup); 

  //initialize my current set
  fd_set CurrentSockets, ReadySocket;
  set_fds(CurrentSockets, server.GetServerFds());

  while (1){

    std::vector<ServerSetup>::iterator server_it;
    std::cout << "waiting for a new connection" << std::endl;
    std::pair<bool, std::pair<int, size_t> > search_fd;
    // select destruct all sockets
    ReadySocket = CurrentSockets;

    //FD_SETSIZE: Max FD Socket Can Support
    if (select(FD_SETSIZE, &ReadySocket, NULL, NULL, NULL) < 0){
      std::cerr << "In select" << std::endl;
      exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < FD_SETSIZE; i++){

      if (FD_ISSET(i, &ReadySocket)){

        if ((search_fd = find_fd(i, server.GetServerFds())).first){
          //this is a new connection
          int client_socket = server.AcceptNewConnection(search_fd.second);
          server_it = it_b+(search_fd.second).second;
          FD_SET(client_socket, &CurrentSockets);
        }
        
        else{
          // Server::handleConnection(i);
          Server::handleConnection(*server_it, i);
          FD_CLR(i, &CurrentSockets);
        }
      }
    }
  }
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


    // ------------------- Parsing Config File ------------------- //

    std::vector<ServerSetup> servers_setup = parseConfig(argc, argv);
    // --------------------- Test Parsing ------------------------ //

    // for (int i = 0; i < (int)servers.size() ;i++)
    //     std::cout << "Server: " << i
    //             << " | Server name1: " << servers[i].getServer_name()[0]
    //             << " | Error pages: " <<  servers[i].getError_pages()[0].second
    //             << " port: "<< servers[i].getListen().first << std::endl;

    // std::cout << "Server: 0 | " << servers[0].getLocations()[1].path << std::endl;
    // std::cout << "Server: 0 | " << servers[0].getLocations()[1].error_pages[0].second << std::endl;

    // --------------------- Run Server --------------------------- //
    run(servers_setup);

	  
    return (EXIT_SUCCESS);
}