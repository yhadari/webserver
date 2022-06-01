#ifndef __REQUEST_INFO__HPP__
#define __REQUEST_INFO__HPP__

#include <iostream>
#include <map>

class RequestInfo
{
    friend class ParserRe;
    private:
    //------ private member --------- >>
    std::string                         _request_method; // Ex: GET
    std::string                         _request_target; // Ex: /download ;URI
    std::string                         _query_string; // Ex: ?id=1
    std::string                         _HTTP_version; // Ex: HTTP/1.1
    std::map<std::string, std::string>  _headers; // Ex: Connection: keep-alive
    std::string                         _body;
    bool                                _is_bad_req;

    public:
    // --------------------------------------------------------- //
    // --------------- Constructors and Operators -------------- //
    // --------------------------------------------------------- //
    RequestInfo();
    RequestInfo(const RequestInfo& request_info);
    RequestInfo&    operator=(const RequestInfo& request_info);


    // --------------------------------------------------------- //
    // -------------------- Getters  --------------------------- //
    // --------------------------------------------------------- //
    std::string                         getRequest_method() const;
    std::string                         getRequest_target() const;
    std::string                         getHTTP_version() const;
    std::map<std::string, std::string>  getHeaders() const;
    std::string                         getBody() const;
    std::string                         getQueryString() const;                                

    // --------------------------------------------------------- //
    // --------------------  Member Methods -------------------- //
    // --------------------------------------------------------- //
    bool                                isBadRequest() const;

}; // class RequestInfo

#endif
