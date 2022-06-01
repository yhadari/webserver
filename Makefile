NAME = webserv

SRC =  	Server.cpp  main.cpp\
		parser_config/Token.cpp \
		parser_config/Lexer.cpp \
		parser_config/SetupServer.cpp \
		parser_config/Parser.cpp \
		parser_request/RequestInfo.cpp \
		parser_request/LexerRe.cpp \
		parser_request/ParserRe.cpp \
		response/Response.cpp \
		utilities/Utils.cpp

all : $(NAME)

$(NAME): $(SRC)
	@c++ -Wall -Wextra -Werror $(SRC) -o $(NAME) -g -fsanitize=address 

clean:
	@ rm -rf *.gch
	@ rm -rf *.dSYM
	@rm -rf $(NAME)
	
fclean: clean

re: fclean all

push : 
	git add .
	git commit -m "$(m)"
	git push origin

test:
	@make re && ./webserv