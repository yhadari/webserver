#include "Response.hpp"
#include <sys/socket.h>
#include "../include/Utils.hpp"
#include "Extensions.hpp"
#include "../cgi/CGI.hpp"
#include "AutoIndex.hpp"
// --------------------------------------------------------- //
// --------------- Constructors and Operators -------------- //
// --------------------------------------------------------- //

Response::Response(int fd_sock_req, RequestInfo request_info, ServerSetup server_setup) 
    : _server_setup(server_setup), _request_info(request_info), _fd_sock_req(fd_sock_req),
       _is_error(false), _type_req_target(IS_NOT_FOUND), _is_location(false)
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
        location = this->_server_setup.getLocation(request_info.getRequest_target(), &_type_req_target);
        if (!location && this->_type_req_target == IS_NOT_FOUND)
        {
                sendErrorPage(404, "File/Location Not Found");
                return ;
        }
        if (location)
            InitResponseConfig(location);
    }
    else
        _is_location = true;
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
    if (this->verifyRequest() == false)
        return (0);
    else if (this->_server_setup.getReturn().first > -1)
        return (redirect());
    else if (this->_request_info.getRequest_method() == "GET")
        return (this->GET());
    else if (this->_request_info.getRequest_method() == "POST")
        return (this->POST());
    else if (this->_request_info.getRequest_method() == "DELETE")
        return (this->DELETE());
    return this->sendErrorPage(405, "Method Not Allowed");
}

int                                     Response::GET()
{   

    std::string path = _server_setup.getRoot() + _request_info.getRequest_target();
    if (this->_type_req_target == IS_FILE)
    {
        std::string uri = _request_info.getRequest_target();
        if (isCGIFile(uri) && _server_setup.getPhpCgiPath().length() > 0)
            path = handle_cgi(_server_setup.getRoot() + uri, _request_info, _server_setup);
        else if (isCGIFile(uri) && _server_setup.getPhpCgiPath().length() == 0)
            return (sendErrorPage(403, "PHP CGI not found"));
        this->ConstructResponseFile(200, "OK", path);
        this->sendResponse();
        if (isCGIFile(uri))
            system("cat /dev/null > /tmp/cgi.html");
        return (1);
    }
    else if (this->_is_location && this->_server_setup.getAutoindex() == "off")
    {
        if ((path = getExistIndex()) != "NOT_FOUND")
        {
            this->ConstructResponseFile(200, "OK", path);
            this->sendResponse();
        }
        else
            this->sendErrorPage(404, "File Not Found");
        return (1);
    }
    else if (this->_server_setup.getAutoindex() == "on")
    {
        this->ConstructResponseFile(200, "OK", autoIndexPath(path, this->_server_setup.getLocationPath())); //path is empty
        this->sendResponse();
        std::remove(AUTO_INDEX_PATH);
        return (1);
    }
    return (this->sendErrorPage(403, "File/Directory Not Found"));
}

int                                     Response::POST()
{   
    std::string uri = _request_info.getRequest_target();
    //  Upload File
    if (this->_server_setup.getUploadStore() != "")
    {
        if (bodyIsFile())
            uploadFile();
        this->ConstructResponseFile(200, "OK", SUCCESS_PAGE_UPLOAD);
        this->sendResponse();
        return (0);
    }
    if (isCGIFile(uri) && _server_setup.getPhpCgiPath().length() > 0)
    {
        std::string path = handle_cgi(_server_setup.getRoot() + uri, _request_info, _server_setup);            
        this->ConstructResponseFile(200, "OK", path);
        this->sendResponse();
        system("cat /dev/null > /tmp/cgi.html");
        return (0);
    }
    return (sendErrorPage(403, "Forbidden"));
}

int                                     Response::deleteFiles(std::string& path) // 0 if success, -1 if error
{   
    DIR             *dir;
    struct dirent   *direntp;
    dir = opendir(path.c_str());
    if (dir == NULL)
        return (std::remove(path.c_str())); // 0 if success
    else
	{
        for(;;)
        {
            direntp = readdir(dir);
            if (direntp == NULL)
                return (2);
            if (direntp->d_type == DT_DIR)
            {
                if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0)
                    continue;
                std::string new_path = path + "/" + direntp->d_name + "/";
                if (deleteFiles(new_path) == -1)
                    return (-1);
                else if (std::remove(new_path.c_str()) == -1)
                    return (-1);
            }
            else if (direntp->d_type == DT_REG)
            {
                std::string file_path = path + "/" + direntp->d_name;
                if (std::remove(file_path.c_str()) == -1)
                    return (-1);
            }
        }
    }
    if (std::remove(path.c_str()) == -1)
        return (-1);
    return (0);
}

int                                     Response::DELETE()
{
    std::string path = _server_setup.getRoot() + _request_info.getRequest_target();

    // int j = std::remove(path.c_str());
    // std::cout << "************* j ************ " << j << "***************"<< path.c_str() << std::endl;

    int i = deleteFiles(path);
    if (i == -1)
        return (sendErrorPage(403, "Forbidden"));
    else if (i == 2)
    {
        if (std::remove(path.c_str()) == -1)
            return (sendErrorPage(403, "Forbidden"));
    }
    this->ConstructResponseFile(200, "OK", SUCCESS_PAGE_DELETE);
    this->sendResponse();
    return (1);
}

void            Response::InitResponseConfig(t_location *location)
{
    _is_location = true;
    this->_server_setup._location_path = location->path;
    if (location->path.length())
        _server_setup.root += location->root; 
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
    if (location->upload_store.length())
        _server_setup.upload_store = location->upload_store;
    if (location->_return.first != -1)
        _server_setup._return = location->_return;
    if (location->_php_cgi_path.length())
         _server_setup._php_cgi_path = location->_php_cgi_path;
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
    else if (status_code == 403)
        return (std::make_pair(ERROR_PAGE_403, "KO"));
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
    std::cout << "appendContentType" << std::endl;
    this->_response_file << "Content-Type: ";
    this->_response_file << getContentType(path);
    this->_response_file << "\r\n";
}

void                Response::appendContentLength(const std::string& path)
{
    std::cout << "appendContentLength" << std::endl;
    this->_response_file << "Content-Length: ";
    this->_response_file << (int)sizeFile(path);
    this->_response_file << "\r\n";
}

void                Response::appendBody(const std::string& path)
{   
    std::cout << "appendBody" << std::endl; // debug
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
    std::cout << "Response sent" << std::endl; // DEBUG
    this->_response_file.open(RESPONSE_FILE_NAME, std::ios::in);

    char tmp_char[LENGHT_SEND_BUFFER];
    int n_read;
    while (!this->_response_file.eof())
    {
        n_read = this->_response_file.read(tmp_char, LENGHT_SEND_BUFFER).gcount();
        if (n_read == 0)
            break ;
        if (send(this->_fd_sock_req, &tmp_char, n_read, 0) <= 0)
            break ;
    }
    // Close the socket request if is not keep-alive
    if (this->_request_info.getHeaders().find("Connection") != this->_request_info.getHeaders().end())
    {
        if (this->_request_info.getHeaders()["Connection"] != "keep-alive")
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
    std::cout << "ERROR: " << status_code << " " << debug_msg << "!" << std::endl;
    ConstructResponseFile(status_code, msg, path);
    this->sendResponse();

    return (false);
}

bool                                    Response::verifyRequest() // false if Response Error Sended
{
    if (this->_is_error)
        return this->sendErrorPage(400, "Bad Request");
    // Check if Method is allowed
    int i;
    for (i = 0; i < (int)this->_server_setup.getRequest_method().size(); i++)
        if (this->_server_setup.getRequest_method()[i] == this->_request_info.getRequest_method())
            break;
    if (this->_server_setup.getRequest_method().size() > 0
        && this->_server_setup.getRequest_method()[i] != this->_request_info.getRequest_method())
        return (sendErrorPage(405)); 
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
        if (this->_request_info.getHeaders()["Connection"] != "keep-alive")
            close(this->_fd_sock_req);
    }
    close(this->_fd_sock_req);
}

std::string                             Response::getExistIndex()
{
    std::string full_path;

    for (int i = 0; i < (int)this->_server_setup.getIndex().size(); i++)
    {
        full_path = this->_server_setup.getRoot() + this->_server_setup.getLocationPath() + this->_server_setup.getIndex()[i];
        if (access(full_path.c_str(), F_OK) != -1) // file exist
           return (full_path);
    }
    return ("NOT_FOUND");
}

bool                              Response::bodyIsFile()
{
    std::string line;
    std::istringstream iss(this->_request_info.getBody().substr(0, 1000)); // 1000byte expect more than 2 lines
    std::getline(iss, line);
    std::getline(iss, line);
    if (line.find("filename") != std::string::npos)
        return (true);
    return (false);
}

int                             Response::uploadFile()
{
    std::string line;
    std::stringstream body(this->_request_info.getBody()); // 1000 byte expect more than 2 lines

    // get delimeter bondry
    std::string first_line;
    std::getline(body, first_line);
    first_line = first_line.substr(0, first_line.length() - 1) + "--\r";
    // std::cout << first_line << std::endl;
    
    // Get file Name
    std::getline(body, line);
    size_t pos = line.find("filename=\"") + 10;
    std::string filename = line.substr(pos, line.length() - pos - 2);

    // Create file name
    std::fstream upload_file(this->_server_setup.getRoot()
                            + this->_server_setup.getLocationPath() + "/" 
                            + this->_server_setup.getUploadStore() + filename, std::fstream::out);
    // skip headers of body
    while (std::getline(body, line))
    {
        if (line.find("\r") == line.length() - 1)
        {
            std::getline(body, line);
            if (line == "\r")
                break;
        }
    }
    // Set file content
    std::getline(body, line);
    std::string tmp;
    if (line != first_line)
        tmp.append(line);
    else
    {
        tmp.erase(tmp.length() - 1, 1);
        upload_file << tmp;
        upload_file.close();
        return (0);
    }
    while (std::getline(body, line))
    {
        if (line != first_line) //diff
            tmp.append("\n");
        else
            break;
        tmp.append(line);
    }
    tmp.erase(tmp.length() - 1, 1);
    upload_file << tmp;
    if (upload_file.is_open())
        upload_file.close();
    return (0);
}

bool                                Response::redirect()
{
    std::string body = "<head><meta http-equiv=\"Refresh\" content=\"0; URL="
                + this->_server_setup.getReturn().second
                + "\"></head>";
    std::string response = "HTTP/1.1 302 Moved Temporarily\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: " + std::to_string(body.length()) + "\r\n\r\n";
    response += body;
    send(this->_fd_sock_req, response.c_str(), response.length(), 0);
    if (this->_request_info.getHeaders().find("Connection") != this->_request_info.getHeaders().end())
    {
        if (this->_request_info.getHeaders()["Connection"] != "keep-alive")
            close(this->_fd_sock_req);
    }
    close(this->_fd_sock_req);
    return (true);
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

bool                            Response::isCGIFile(const std::string& uri)
{
    if (uri.substr(uri.find_last_of(".") + 1) == "php")
        return (true);
    return (false);
}