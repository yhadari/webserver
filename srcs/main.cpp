#include "include/Utils.hpp"
#include "include/Server.hpp"
#include "response/AutoIndex.hpp"

// server add currentSocket to attribute
void run(std::vector<ServerSetup> servers_setup)
{
	std::vector<ServerSetup>::iterator it_b(servers_setup.begin());
	Server server(servers_setup);

	// initialize my current set
	fd_set SocketsRead, ReadyForRead;
	fd_set SocketsWrite, ReadyForWrite;
	set_fds(SocketsRead, SocketsWrite, server.GetServerFds());

	while (1)
	{
		std::vector<ServerSetup>::iterator server_it;
		std::cout << "Waiting for a new connection" << std::endl;
		std::pair<bool, std::pair<int, size_t> > search_fd;
		// select destruct all old sockets
		ReadyForRead = SocketsRead;
		ReadyForWrite = SocketsWrite;
		// FD_SETSIZE: Max FD Socket Can Support
		if (select(FD_SETSIZE, &ReadyForRead, &ReadyForWrite, NULL, NULL) < 0)
		{
			std::cerr << "In select" << std::endl;
			exit(EXIT_FAILURE);
		}
		for (size_t i = 0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &ReadyForRead))
			{
				if ((search_fd = find_fd(i, server.GetServerFds())).first)
				{
					// this is a new connection
					int client_socket = server.acceptNewConnection(search_fd.second);
					server_it = it_b + (search_fd.second).second;
					FD_SET(client_socket, &SocketsRead);
				}
				else
				{
					if (!server.isRequestExist(i))
						server.addNewRequest(i);
					if (server.handleConnection(*server_it, i) == true)
						FD_CLR(i, &SocketsRead);
				}
			}
			else if (FD_ISSET(i, &ReadyForWrite))
			{
				if ((search_fd = find_fd(i, server.GetServerFds())).first)
				{
					// this is a new connection
					int client_socket = server.acceptNewConnection(search_fd.second);
					server_it = it_b + (search_fd.second).second;
					FD_SET(client_socket, &SocketsWrite);
				}
				else
				{
					if (!server.isRequestExist(i))
						server.addNewRequest(i);
					if (server.handleConnection(*server_it, i) == true)
						FD_CLR(i, &SocketsWrite);
				}
			}
		}
	}
}

std::vector<ServerSetup> parseConfig(int argc, char **argv, char ***envp)
{
	std::string contents;
	if (argc == 2)
		contents = fileToSring(argv[1]);
	else
		contents = fileToSring(std::string("default.config").c_str());
	Lexer lexer(contents);
	Parser parser(lexer);
	return (parser.parse(envp));
}

int main(int argc, char **argv, char **envp)
{
	// ------------------- Parsing Config File ------------------- //
	std::vector<ServerSetup> servers_setup = parseConfig(argc, argv, &envp);
	signal(SIGPIPE, SIG_IGN); // ignore SIGPIPE // fix problem with broken pipe when client close connection

	// --------------------- Run Server --------------------------- //
	run(servers_setup);
	return (EXIT_SUCCESS);
}