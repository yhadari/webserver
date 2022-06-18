NAME = webserv

SRC =  	srcs/Server.cpp srcs/Request.cpp srcs/main.cpp\
		srcs/parser_config/Token.cpp \
		srcs/parser_config/Lexer.cpp \
		srcs/parser_config/SetupServer.cpp \
		srcs/parser_config/Parser.cpp \
		srcs/parser_request/RequestInfo.cpp \
		srcs/parser_request/LexerRe.cpp \
		srcs/parser_request/ParserRe.cpp \
		srcs/response/Response.cpp \
		srcs/utilities/Utils.cpp \
		srcs/cgi/CGI.cpp \
		srcs/response/AutoIndex.cpp

all : $(NAME)

$(NAME): $(SRC)
	@c++ -Wall -Wextra -Werror $(SRC) -o $(NAME) 

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