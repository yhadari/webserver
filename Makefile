NAME = webserv
SRC =  	main.cpp \
		parser_config/Token.cpp \
		parser_config/Lexer.cpp \
		parser_config/SetupServer.cpp \
		parser_config/Parser.cpp \
		parser_config/utils.cpp \
		server/test.cpp

all : $(NAME)

$(NAME): $(SRC)
	@clear
	@c++ -Wall -Wextra -Werror $(SRC) -o $(NAME)

clean:
	@ rm -rf *.gch
	@ rm -rf *.dSYM
	@rm -rf $(NAME)
	
fclean: clean

re: fclean all