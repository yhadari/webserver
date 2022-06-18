#ifndef SERVER_HPP
#define SERVER_HPP

#include "Utils.hpp"
#include "Request.hpp"
#include <map>

#define LENGTH_RECV_BUFFER 2048

class Server
{
	private:
	std::vector<struct sockaddr_in> _v_address;
	std::vector<int> 				_v_server_fd;
	std::map<int, Request> 			_requests;
	std::vector<ServerSetup> 		_servers_setup;

	int 							_server_fd;
	struct sockaddr_in 				_address;
	int 							_address_len;
	int 							opt;

	public:
	// ================= Constructor =============== //
	Server(std::vector<ServerSetup> servers);

	// ================ Getter ===================== //
	std::vector<int> 		GetServerFds();

	// ============= Member function ============== //
	int 					acceptNewConnection(std::pair<int, size_t> pair);
	bool 					handleConnection(ServerSetup server_setup, int new_socket);
	Request 				receiveRequest(int new_socket);
	void 					addNewRequest(int fd);
	void					removeRequest(int fd);
	bool					isRequestExist(int fd);
	int						getContentLength(char *buffer);
	ServerSetup				checkServerSetup(std::string server_name);
};

#endif