#ifndef __PARSER_RE__HPP__
#define __PARSER_RE__HPP__

#include "LexerRe.hpp"
#include "RequestInfo.hpp"


// Define Errors Messages


// class ParserRe
class ParserRe
{
    public:
    //------ public member ------------------------------------- //
    LexerRe lexer;
    Token   curr_token;
    Token   prev_token;

    // --------------------------------------------------------- //
    // --------------- Constructors and Operators -------------- //
    // --------------------------------------------------------- //
    ParserRe(LexerRe lexer);
    ParserRe(const ParserRe& parser);
    ParserRe& operator=(const ParserRe& parser);

    // --------------------------------------------------------- //
    // ----------------- Member Methods ------------------------ //
    // --------------------------------------------------------- //
    void                                        eat(TypeToken token_type);
    RequestInfo                                 parse();
    std::pair<std::string, std::string>         parseHeader();

    // --------------------------------------------------------- //
    // -------------- Non Member Functions --------------------- //
    // --------------------------------------------------------- //
    static  void errorDisplay(std::string s);

}; // class ParserRe

#endif