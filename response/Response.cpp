#include "Response.hpp"
#include <sys/socket.h>
#include "../include/Utils.hpp"
#include "Extensions.hpp"

// --------------------------------------------------------- //
// --------------- Constructors and Operators -------------- //
// --------------------------------------------------------- //

Response::Response(int fd_sock_req, RequestInfo request_info, ServerSetup server_setup) 
    : _server_setup(server_setup), _request_info(request_info), _fd_sock_req(fd_sock_req),
       _type_req_target(IS_LOCATION)
{
    this->_response_file.open("response.temp", std::ios::out); // Open File
    if (!_response_file.is_open())
    {    
            this->senUnxpectedError();
            return ;
    }
    if (request_info.getRequest_target() != "/")
    {
        t_location *location;
        if (request_info.isBadRequest())
        {   
            sendErrorPage(400, "Bad Request");
            return ;
        }
        else if (!(location = this->_server_setup.getLocation(request_info.getRequest_target(), &_type_req_target)))
        {
            if (this->_type_req_target == IS_NOT_FOUND)
            {
                sendErrorPage(404, "File/Location Not Found");
                return ;
            }
        }
        if (location)
            InitResponseConfig(location);
    }
}


Response::~Response() // Memory Leaks if exist
{
    // Remove the temp response file
    if(_response_file.is_open())
        _response_file.close();
    std::remove(RESPONSE_FILE_NAME);
}

// --------------------------------------------------------- //
// --------------------  Member Methods -------------------- //
// --------------------------------------------------------- //
bool                                    Response::IsSended()
{
    return (this->_is_error);
}

int                                     Response::handleResponse()
{
    if (this->_is_error || !this->verifyRequest())
        return (1);
    if (this->_request_info.getRequest_method() == "GET")
        return (this->GET());
    else if (this->_request_info.getRequest_method() == "POST")
        return (this->GET());
    else if (this->_request_info.getRequest_method() == "DELETE")
        return (this->DELETE());
    return this->sendErrorPage(405, "Method Not Allowed");
}

int                                     Response::GET()
{   
    std::string path;
    if (this->_type_req_target == IS_FILE)
    {
        std::string uri = _request_info.getRequest_target();
        if (uri.compare(uri.find_last_of('.'), uri.length(), ".php") != (int)std::string::npos)
            path = cgi_ft(_server_setup.getRoot() + uri);

        else
            path = this->_server_setup.getRoot() + this->_request_info.getRequest_target();

        this->ConstructResponseFile(200, "OK", path);
        this->sendResponse();
        if (uri.compare(uri.find_last_of('.'), uri.length(), ".php") != (int)std::string::npos)
            system("cat /dev/null > cgi.html");
    }
    else if (this->_type_req_target == IS_LOCATION && this->_server_setup.getAutoindex() == "off")
    {
        std::string path;

        if ((path = getExistIndex()) != "NOT_FOUND")
        {
            this->ConstructResponseFile(200, "OK", path);
            this->sendResponse();
        }

        else
            return (this->sendErrorPage(404, "File Not Found"));
    }
    else if (this->_type_req_target == IS_LOCATION && this->_server_setup.getAutoindex() == "on")
    {
        // function auto index;
        return (this->sendErrorPage(403, "Forbidden"));// Not created yet
    }
    return (1);
}

int                                     Response::POST()
{
    return this->sendErrorPage(405, "Method Not Allowed");
    return (1);
}

int                                     Response::DELETE()
{
    return this->sendErrorPage(405, "Method Not Allowed");
    return (1);
}

void            Response::InitResponseConfig(t_location *location)
{
    if (location->path.length())
        _server_setup.root += (location->path + location->root);
    if (!location->index.empty())
        _server_setup.index = location->index;
    if (!location->error_pages.empty())
        _server_setup.error_pages = location->error_pages;
    if (location->client_max_body_size != -1)
        _server_setup.client_max_body_size = location->client_max_body_size;
    if (!location->request_method.empty())
        _server_setup.request_method = location->request_method;
    if (location->autoindex.length())
         _server_setup.autoindex = location->autoindex;
}

std::pair<std::string, std::string>    Response::getErrorPage(int status_code) // (pair(path, msg))
{
    // Check in the config file;
    std::vector<std::pair<short, std::string> > v = _server_setup.getError_pages();
    for(int i = 0; i < (int)v.size(); i++)
        if (v[i].first == status_code)
            return (std::make_pair(_server_setup.getRoot() + v[i].second, "OK"));
    if (status_code == 404)
        return (std::make_pair(ERROR_PAGE_404, "Page_Not_Found"));
    else if (status_code == 500)
        return (std::make_pair(ERROR_PAGE_500, "KO"));
    else if (status_code == 400)
        return (std::make_pair(ERROR_PAGE_400, "KO"));
    else if (status_code == 405)
        return (std::make_pair(ERROR_PAGE_405, "KO"));
    else if (status_code == 413)
        return (std::make_pair(ERROR_PAGE_413, "KO"));
    return (std::make_pair(ERROR_PAGE_404, "KO")); // default
}


void               Response::appendStartLine(int status_code, const std::string& msg)
{
    this->_response_file << "HTTP/1.1 ";
    this->_response_file << status_code;
    this->_response_file << " " + msg;
    this->_response_file << "\r\n";
}

void                Response::appendContentType(const std::string& path)
{
    this->_response_file << "Content-Type: ";
    this->_response_file << getContentType(path);
    this->_response_file << "\r\n";
}

void                Response::appendContentLength(const std::string& path)
{
    this->_response_file << "Content-Length: ";
    this->_response_file << (int)sizeFile(path);
    this->_response_file << "\r\n";
}

void                Response::appendBody(const std::string& path)
{
    std::ifstream   in_file(path);
    std::string     tmp_line;

    if (!in_file.is_open())
            return ;
    this->_response_file << "\r\n";

    while (std::getline(in_file, tmp_line))
    {
        this->_response_file << tmp_line;
        if (!in_file.eof())
            this->_response_file << "\n";
    }
    if (in_file.is_open())
        in_file.close();
    if (this->_response_file.is_open())
        this->_response_file.close();
}

void                                    Response::sendResponse()
{
    this->_response_file.open(RESPONSE_FILE_NAME, std::ios::in);

    char tmp_char[LENGHT_SEND_BUFFER];
    int n_read;
    while (!this->_response_file.eof())
    {
        n_read = this->_response_file.read(tmp_char, LENGHT_SEND_BUFFER).gcount();
        if (n_read == 0)
            break ;
        send(this->_fd_sock_req, &tmp_char, n_read, 0);
    }
    // Close the socket request if is not keep-alive
    if (this->_request_info.getHeaders().find("Connection") != this->_request_info.getHeaders().end())
    {
        if (this->_request_info.getHeaders().at("Connection") != "keep-alive")
            close(this->_fd_sock_req);
    }
    close(this->_fd_sock_req);
}

void                                    Response::ConstructResponseFile(int status_code, const std::string& msg, const std::string& path)
{
    this->appendStartLine(status_code, msg);
    this->appendContentType(path);
    this->appendContentLength(path);
    this->appendBody(path);
}

bool                                    Response::sendErrorPage(int status_code, std::string debug_msg = "")
{
    std::pair<std::string, std::string>   error_page = getErrorPage(status_code);
    std::string                           path = error_page.first;
    std::string                           msg = error_page.second;

    this->_is_error = true;
    ConstructResponseFile(status_code, msg, path);
    this->sendResponse();

    std::cout << "ERROR: " << status_code << " " << debug_msg << "!" << std::endl;
    return (false);
}


bool                                    Response::verifyRequest() // false if Response Error Sended
{
    // Check if Method is allowed
    int i;
    for (i = 0; i < (int)this->_server_setup.getRequest_method().size(); i++)
        if (this->_server_setup.getRequest_method()[i] == this->_request_info.getRequest_method())
            break;
    if (i == (int)this->_server_setup.getRequest_method().size())
        return (sendErrorPage(405));

    // check auto index priority
    // Function TO DO    

    // Check body size < client_max_body_size
    int max_size;
    if ((max_size = this->_server_setup.getClient_max_body_size()) != -1)
    {
        if ((int)this->_request_info.getBody().length() > max_size)
            return (sendErrorPage(413)); // create 413 error page
    }
    
    return (true);
}

// send char * to client and set the _is_error to true
void                                    Response::senUnxpectedError()
{
    std::string body = "<h1>Internal Server Error 500</h1>";
    std::string response_error;

    this->_is_error = true;
    response_error = "HTTP/1.1 500 Internal Server Error\r\n";
    response_error += "Content-Type: text/html\r\n";
    response_error += "Content-Length: " + std::to_string(body.length()) + "\r\n";
    response_error += "\r\n";
    response_error += body;
    send(this->_fd_sock_req, response_error.c_str(), response_error.length(), 0);

    // Close the socket request if is not keep-alive
    if (this->_request_info.getHeaders().find("Connection") != this->_request_info.getHeaders().end())
    {
        if (this->_request_info.getHeaders().at("Connection") != "keep-alive")
            close(this->_fd_sock_req);
    }
}

std::string                             Response::getExistIndex()
{
    std::string full_path;

    for (int i = 0; i < (int)this->_server_setup.getIndex().size(); i++)
    {
        full_path = this->_server_setup.getRoot() + this->_server_setup.getIndex()[i];
        if (access(full_path.c_str(), F_OK) != -1) // file exist
           return (full_path);
    }
    return ("NOT_FOUND");
}

std::string Response::cgi_ft(std::string path){

    std::vector<const char*> argv;
    std::vector<const char*> envp;

    envp.push_back(strdup((std::string("REQUEST_METHOD") + "=" + this->_request_info.getRequest_method()).c_str()));
    std::ostringstream ss;
    ss << (this->_request_info.getBody()).length();
	envp.push_back(strdup((std::string("CONTENT_LENGTH") + "=" + ss.str()).c_str()));
	envp.push_back(strdup((std::string("CONTENT_TYPE") + "=text.html").c_str()));
	envp.push_back(strdup((std::string("GATEWAY_INTERFACE") + "=CGI/1.1").c_str()));
	envp.push_back(strdup((std::string("SERVER_NAME") + "=" + this->_server_setup.getServer_name()[0]).c_str()));
    ss << this->_server_setup.getListen().first;
	envp.push_back(strdup((std::string("SERVER_PORT") + "=" + ss.str()).c_str()));
	envp.push_back(strdup((std::string("SERVER_PROTOCOL") + "=HTTP/1.1").c_str()));
	envp.push_back(strdup((std::string("SERVER_SOFTWARE") + "=" + "SERVER_NAME").c_str()));
	envp.push_back(strdup((std::string("QUERY_STRING") + "=" + this->_request_info.getQueryString()).c_str()));

	// envp.push_back(strdup((std::string("PATH_INFO") + "=" + this->_request_info.getRequest_target()).c_str()));
	// envp.push_back(strdup((std::string("PATH_TRANSLATED") + "=" + this->_server_setup.getRoot()
    //         + this->_request_info.getRequest_target()).c_str()));
    ss << this->_server_setup.getListen().second;
	envp.push_back(strdup((std::string("REMOTE_ADDR") + "=" + ss.str()).c_str()));
	envp.push_back(strdup((std::string("REMOTE_HOST") + "=" + "localhost").c_str()));

    envp.push_back(strdup((std::string("SCRIPT_NAME") + "=" + this->_request_info.getRequest_target()).c_str()));

    // envp.push_back(strdup((std::string("QUERY_STRING") + "=" + this->_request_info.getBody()).c_str()));
	// envp.push_back(strdup((std::string("HTTP_COOKIE") + "=" + req.getHeader("Cookie")).c_str()));
	envp.push_back(NULL);

    argv.push_back(strdup("/usr/bin/php"));
    argv.push_back(strdup(path.c_str()));
    argv.push_back(NULL);

    std::string file = "cgi.html";
    // std::fstream in_file("body_req.txt");
    // in_file << this->_request_info.getBody();
    // in_file.close();
    
    int fd = open(file.c_str(), O_CREAT|O_RDWR, 0777);
    // int fd_in = open("body_req.txt", O_CREAT|O_RDONLY, 0777);



    pid_t process = fork();

    if (process == 0)
    {
        dup2(fd, 1);
        // dup2(fd_in, 0);
        execve(argv[0], const_cast<char * const *>(argv.data()), const_cast<char * const *>(envp.data()));
        exit (0);
    }
    return file;
}

// --------------------------------------------------------- //
// ------------------  Non Member Functions ---------------- //
// --------------------------------------------------------- //

std::string                     Response::getContentType(const std::string& full_path)
{
    std::string path = full_path.substr(full_path.find_last_of('.') + 1);
    if (path.size() == full_path.size())
        return "unknown";
    else
    {
        std::map<std::string,std::string>::iterator it = extension.find(path);
        if (it != extension.end())
            return it->second;
    }
    return "unknown";
}



// ----------------------------- TEST !!!!! ---------------------------------------//
// ----------------------------- TEST !!!!! ---------------------------------------//
// std::string  Response::test(RequestInfo request_info, ServerSetup server_setup){

//     //1 -  verify any method (GET POST DELETE) //     // and exist in config file
//     // Verifiez extention of file if exist
//     std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
//     // Verifier root && the file if exist
//     // Calculate Lenght of the file;
//     if (request_info.getRequest_method() == "GET")
//     {   
//         std::string path;
//         // if root
//         if (request_info.getRequest_target() == "/about")
//             path = server_setup.getRoot() + server_setup.getLocations()[0].path + "/" + server_setup.getLocations()[0].index[0];
//         else // if location
//             path = server_setup.getRoot() + "/" + server_setup.getIndex().at(0);
        
//         std::ifstream index_file(path);

//         if (!index_file.is_open())
//             return (0);
    
//         std::cout << "\n\n/<< ****************** Index Content : ********************* >>"<< std:: endl;
//         std::string response_line;
//         std::string body;
//         int         size_body = 0;
//         while (std::getline(index_file, response_line))
//         {
//             // std::istringstream iss(response_line);
//             // int a, b;
//             // if (!(iss >> a >> b)) { break; } // error
//             body.append(response_line);
//             size_body += response_line.length();
//             if (!index_file.eof())
//             {
//                 body.append("\n");
//                 size_body++;
//             }
//         }
//         response.append(std::to_string(size_body));
//         response.append("\r\n\r\n");
//         response.append(body);
//         index_file.close();
//     }
//     return (response);
// }