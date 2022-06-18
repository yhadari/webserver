#ifndef __REQUEST_HPP__
#define __REQUEST_HPP__

#include <iostream>
#include "../parser_request/include/ParserRe.hpp"
#include "../parser_config/include/ServerSetup.hpp"

class Request
{
    private:
    int         _content_length;
    int         _readed_body;
    std::string _buffer;
    bool        _isHeaderReaded;
    bool        _is_chanked;
    std::string _server_name;
    ServerSetup _server_setup;
    bool        _is_complete;


    public:
    Request();

    // Getters && Setters
    int         getContentLength();
    void        setContentLength(int content_length);
    int         getReadBody();
    std::string getBuffer();
    void        setHeaderReaded(bool isHeaderReaded);
    void        setServerSetup(const ServerSetup& server_setup);
    ServerSetup getServerSetup();
    std::string getServerName();
    bool        isComplete();


    // Member functions
    void        appandBuffer(char * buffer, int length);
    bool        isHeaderReaded();
    int		    calculateReadedBody(char *buffer, int length);
    int         setHeaders(char *buffer);
    bool        isChanked();
    void        deleteDelimeter(bool begin);
    void        makeComplete();

};

#endif