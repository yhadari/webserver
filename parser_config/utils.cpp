#include "include/utils.hpp"
#include <string>

int     stringToInt(const std::string s)
{
    for (int i = 0; i < (int)s.length() ; i++)
    {
        if (!std::isdigit(s[i]))
        {
            std::cerr << "\"" << s << "\" Not a valid Number!" << std::endl;
            exit(EXIT_FAILURE);
        }
    }   
    return (atoi(s.c_str()));
}

bool isNumber(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}