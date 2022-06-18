#ifndef __RESPONSE_HPP__
#define __RESPONSE_HPP__

#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <cstdio> // remove file response
#include <unistd.h> // close

#include "../parser_config/include/ServerSetup.hpp"
#include "../parser_request/include/RequestInfo.hpp"
#include "../include/EnumRequestTarget.hpp"

// Defines 
#define RESPONSE_FILE_NAME "response.temp"
#define LENGHT_SEND_BUFFER 1024
// Defines Error Pages
#define ERROR_PAGE_404 "error_pages/404_NOT_FOUND.html"
#define ERROR_PAGE_500 "error_pages/500_SERVER_ERROR.html"
#define ERROR_PAGE_400 "error_pages/400_BAD_REQUEST.html"
#define ERROR_PAGE_405 "error_pages/405_METHOD_NOT_ALLOWED.html"
#define ERROR_PAGE_413 "error_pages/413_REQUEST_TOO_LARGE.html"
#define ERROR_PAGE_403 "error_pages/403_FORBIDDEN.html"
#define SUCCESS_PAGE_UPLOAD "success_pages/upload.html"
#define SUCCESS_PAGE_DELETE "success_pages/delete.html"


class Response
{
private:
    //------ private member --------- >>
    ServerSetup         _server_setup;      // Precis config of the request
    RequestInfo         _request_info;      // Request Info
    int                 _fd_sock_req;       // FD of Socket Request
    std::fstream        _response_file;     // temp "response http message file"
    bool                _is_error;          // In response
    TypeRequestTarget   _type_req_target;   // LOCATION, FILE, PHP_FILE or NOT_FOUND
    bool                _is_location;

public:
    // --------------------------------------------------------- //
    // --------------- Constructors and Operators -------------- //
    // --------------------------------------------------------- //
    Response(int fd_sock_req, RequestInfo request_info, ServerSetup server_setup);
    ~Response();

    // --------------------------------------------------------- //
    // --------------------  Member Methods -------------------- //
    // --------------------------------------------------------- //
    bool                                    IsSended();

    void                                    InitResponseConfig(t_location *location);
    std::pair<std::string, std::string>     getErrorPage(int status_code); //  Get (path,msg)
    void                                    ConstructResponseFile(int status_code, const std::string& msg, const std::string& path); // Create Response File
    void                                    appendStartLine(int status_code, const std::string& msg);
    void                                    appendContentType(const std::string& path);
    void                                    appendContentLength(const std::string& path);
    void                                    appendBody(const std::string& path);
    void                                    sendResponse();
    bool                                    sendErrorPage(int status_code, std::string debug_msg);
    int                                     handleResponse();
    bool                                    verifyRequest();
    void                                    senUnxpectedError(); // send char * to client and set the _is_error to true
    std::string                             getExistIndex(); // check index that we can find in path
    int                                     GET();
    int                                     POST();
    int                                     DELETE();
    int                                     uploadFile();
    bool                                    bodyIsFile();
    bool                                    redirect();
    int                                     deleteFiles(std::string& path);

    // --------------------------------------------------------- //
    // ------------------  Non Member Functions ---------------- //
    // --------------------------------------------------------- //
    static std::string                     getContentType(const std::string& full_path);
    static bool                            isCGIFile(const std::string& uri);

    // --------------------  Test Methods ---------------------- //
    // std::string     test(RequestInfo request_info, ServerSetup server_setup);
    
};

#endif // !RESPONSE_HPP
