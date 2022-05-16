NAME = webserv

all : $(NAME)

$(NAME): main.cpp
	@clear
	@c++ -Wall -Wextra -Werror main.cpp -o $(NAME)

clean:
	@ rm -rf *.gch
	@ rm -rf *.dSYM
	@rm -rf $(NAME)
	
fclean: clean

test: 
	@clear
	@c++ -Wall -Wextra -Werror test.cpp -o $(NAME)

re: fclean all