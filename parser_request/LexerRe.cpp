#include "include/LexerRe.hpp"

// Constructors
LexerRe::LexerRe(std::string contents)
{
    this->contents = contents;
    this->i = 0;
    this->c = contents[this->i];

    this->n_word = 0;
    this->is_key = 1;
}

LexerRe::LexerRe()
{
    this->contents = "";
    this->i = 0;
    this->c = contents[this->i];
}

LexerRe::~LexerRe()
{
    this->n_word = 0;
    this->is_key = 1;
}

// Operations
LexerRe::LexerRe(const LexerRe& lexer)
{
    *this = lexer;
}

LexerRe& LexerRe::operator=(const LexerRe& lexer)
{
    this->contents = lexer.contents;
    this->c = lexer.c;
    this->i = lexer.i;
    this->is_key = lexer.is_key;
    this->n_word = lexer.n_word;
    return (*this);
}

// Member Functions
void    LexerRe::advance()
{
    if (this->c != '\0' && this->i < this->contents.length())
	{
		this->i += 1;
		this->c = this->contents[this->i];
	}
}

void	LexerRe::skipWhiteSpaces()
{
    while (this->c == ' ' || this->c == '\t' || this->c == '\n' || this->c == '\r')
		advance();
}

Token   LexerRe::getNextToken()
{
    while (this->c != '\0' && this->i < this->contents.length())
	{
        if (isBody())
            return (collectBody());
		else if (c == ' ' || c == '\r' || c == '\n' || c == '\t')
			skipWhiteSpaces();
		else if (this->c == ':')
			return (advanceWithToken(Token(TOKEN_COLON, ":")));
        else
            return (collectWord());
	}
	return (Token(TOKEN_EOF, "\0"));
}

Token   LexerRe::collectWord()
{
    std::string value;
  
    
    if (this->n_word < 3) // first line 3 param
    {
        this->n_word++;
        while(isWord(this->c))
        {
            value.push_back(this->c);
            advance();
        }
        return (Token(WORD, value));
    }
    while(c != '\r' && c != '\n' && c != '\0')
    {
        if (this->is_key == 1 && c == ':')
            break;
        value.push_back(this->c);
        advance();
    }
    if (this->is_key == 1)
        this->is_key = 0;
    else
        this->is_key = 1;
    return (Token(WORD, value));
}

Token   LexerRe::collectBody()
{
    std::string value;

    skipWhiteSpaces();
    while(this->c != '\0' && this->i < this->contents.length())
    {
        value.push_back(this->c);
        advance();
    }
    return (Token(TOKEN_BODY, value));
}

Token   LexerRe::advanceWithToken(Token token)
{
    advance();
	return (token);
}

int	LexerRe::isWord(char c)
{
	if (c != ' ' && c != '\r'
        && c != '\n' && c != '\0' && c != '\t')
		return (1);
	return (0);
}

bool    LexerRe::isBody()
{
    if ((contents.length() - i) > 3 && contents[i] == '\r'
            && contents[i + 1] == '\n'
            && contents[i + 2] == '\r'
            && contents[i + 3] == '\n')
        return (true);
    return (false);
}