#include "Response.hpp"

Response::Response(){

}

Response::Response(std::string status){
    code_status = status.substr(0, 3);
    phrase_status = status.substr(4);
}

Response::~Response(){

}

void Response::append_header(std::string fst, std::string scd){
    _headers.insert(std::make_pair(fst, scd));
}

int Response::Get_body_size(){
    return(_body.size());
}

std::string Response::make_header(){
    std::string rst;
    //"HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!
    rst.append("HTTP/1.1" + code_status + " OK" + "\r\n" + "Content-Type: text/html\r\n");
    for (std::map<std::string, std::string>::iterator itr = _headers.begin(); itr != _headers.end(); itr++)
        rst.append((*itr).first + ": " + (*itr).second + "\r\n");
    rst.append("\r\n");
    return (rst);
}

void Response::make_body_status(){
	
    _body.clear();
    
    std::string rst;

    rst.append("<!DOCTYPE html><html><head><meta charset=\"UTF-8\"/><title>WebServ</title></head>");
	rst.append("<body>");
	rst.append("<h1>" + code_status + "</h1>");
	rst.append("<h3>" + phrase_status + "</h3>");
    //if return to home oplig decomment the line buttom
	// rst.append("<p>Click <a href=\"/\">here</a> to return home.</p>");
	rst.append("</body></html>");
	
	_body = rst;
}

void Response::make_body_status(std::string url){
    _body.clear();
    std::string rst;

    rst.append(url);
    _body = rst;
}

std::string Response::serialize(){
    std::string rst;

    rst.append("HTTP/1.1 " + code_status + " OK" + "\n" + "Content-Type: text/plain\n" + "Content-Length: 1\n");
    for (std::map<std::string, std::string>::iterator itr = _headers.begin(); itr != _headers.end(); itr++)
        rst.append((*itr).first + ": " + (*itr).second + "\n");
    rst.append("\n");
    rst.append(_body);

    return (rst);
    
}

std::string Response::handleResponse(){

    // std::ofstream Myfile("ResponseFile.txt");
    code_status = "200";
    // append_header("Host", "127.0.0.1:8000");
    // make_header();
    // make_body_status();
    _body = "Hello world!";

    // Myfile << Resp.serialize();
    // Myfile.close();
    return(serialize());
}