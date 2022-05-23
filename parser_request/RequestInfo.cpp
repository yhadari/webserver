#include "include/RequestInfo.hpp"

// --------------------------------------------------------- //
// --------------- Constructors and Operators -------------- //
// --------------------------------------------------------- //
RequestInfo::RequestInfo()
{
    this->_request_method = std::string();
    this->_request_target = std::string();
    this->_HTTP_version = std::string();
    this->_headers = std::map<std::string, std::string>();
    this->_body = std::string();
}

RequestInfo::RequestInfo(const RequestInfo& request_info)
{
    *this = request_info;
}

RequestInfo&    RequestInfo::operator=(const RequestInfo& request_info)
{
    this->_request_method = request_info._request_method;
    this->_request_target = request_info._request_target;
    this->_HTTP_version = request_info._HTTP_version;
    this->_headers = request_info._headers;
    this->_body = request_info._body;

    return (*this);
}

// --------------------------------------------------------- //
// -------------------- Getters  --------------------------- //
// --------------------------------------------------------- //
std::string                         RequestInfo::getRequest_method() const
{
    return (this->_request_method);
}

std::string                         RequestInfo::getRequest_target() const
{
    return (this->_request_target);
}

std::string                         RequestInfo::getHTTP_version() const
{
    return (this->_HTTP_version);
}

std::map<std::string, std::string>  RequestInfo::getHeaders() const
{
    return (this->_headers);
}

std::string                         RequestInfo::getBody() const
{
    return (this->_body);
}

