#ifndef __LEXER_RE_HPP__
#define __LEXER_RE_HPP__

#include "../../parser_config/include/Token.hpp"

class LexerRe
{
    private:
        /* data */
        char			c;
        unsigned int	i;
        std::string		contents;

        int n_word; // if first line parse it the 3 words reset to 0 again
        int is_key;

    public:   
        // Constructors
        LexerRe(std::string contents);
        LexerRe();
        ~LexerRe();
        // Operations
        LexerRe(const LexerRe& lexer);
        LexerRe& operator=(const LexerRe& lexer);

        // Member Methods
        void    advance();
        void	skipWhiteSpaces();
        Token   getNextToken();
        Token   collectWord();
        Token   advanceWithToken(Token token);
        Token   collectBody();
        bool    isBody();

        // Non Member Methods
        static int	isWord(char c);
};

#endif