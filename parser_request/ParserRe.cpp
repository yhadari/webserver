#include "include/ParserRe.hpp"

// --------------------------------------------------------- //
// --------------- Constructors and Operators -------------- //
// --------------------------------------------------------- //

ParserRe::ParserRe(LexerRe lexer) : lexer(lexer)
{
    this->curr_token = this->lexer.getNextToken();
    this->prev_token = this->curr_token;
}

ParserRe::ParserRe(const ParserRe& parser)
{
    *this = parser;
}

ParserRe&    ParserRe::operator=(const ParserRe& parser)
{
    this->lexer = parser.lexer;
    this->curr_token = parser.curr_token;
    this->prev_token = parser.prev_token;
    return (*this);
}


// --------------------------------------------------------- //
// ----------------- Member Methods ------------------------ //
// --------------------------------------------------------- //

void    ParserRe::eat(TypeToken token_type)
{
    if (this->curr_token.type == token_type)
	{
		this->prev_token = this->curr_token;
		this->curr_token = this->lexer.getNextToken();
	}
	else
	{
		std::cerr  << "Unexepected token '" << this->curr_token.value
                << "', with type " << this->curr_token.type << std::endl;
		exit(EXIT_FAILURE);
	}
}
 
RequestInfo    ParserRe::parse() // error multi server
{
    RequestInfo requestInfo;

    // Parse start Line
    this->eat(WORD);
    requestInfo._request_method = prev_token.value;
    this->eat(WORD);
    requestInfo._request_target = prev_token.value;
    this->eat(WORD);
    requestInfo._HTTP_version = prev_token.value;

    while (curr_token.type != TOKEN_BODY)
        requestInfo._headers.insert(parseHeader());
    requestInfo._body = curr_token.value;
    return (requestInfo);
}

std::pair<std::string, std::string>          ParserRe::parseHeader()
{
    std::string key = this->curr_token.value;
    this->eat(WORD);
    this->eat(TOKEN_COLON);

    std::string value = this->curr_token.value;
    this->eat(WORD);

    return (std::make_pair(key, value));
}


//-------------------------------- Non Member Methods ----------------------- //

void ParserRe::errorDisplay(std::string s)
{
    std::cerr << "Error: " << s << std::endl;
    exit(EXIT_FAILURE);
}