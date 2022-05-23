#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <map>
#include <string>
#include <fstream>


class Response
{
private:
    std::string _body;
    std::map<std::string, std::string> _headers;

public:
    std::string code_status;
    std::string phrase_status;
    Response();
    Response(std::string status);
    ~Response();

    void    append_header(std::string fst, std::string scd);
	int     Get_body_size();

	void    make_body_status();
	void    make_body_status(std::string url);
	std::string  make_header();
	std::string  serialize();
    std::string  handleResponse();
};

#endif // !RESPONSE_HPP
