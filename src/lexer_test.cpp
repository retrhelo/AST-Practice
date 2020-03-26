#include <iostream>
#include <fstream>
#include <string>

#include "../lexer"

int main(void) {
	std::string str;

	std::cout << "filename: ";
	std::cin >> str;

	std::ifstream ifstrm(str);
	Lexer lex(ifstrm);

	lex.next();
	while (lex.get_type() != tokenEOF) {
		std::cout << "token: " << lex.get_token() << std::endl;
		std::cout << "type: " << lex.get_type() << std::endl;
		std::cout << "line: " << lex.get_line() << ", " << lex.get_lineChar()
			<< std::endl;
		lex.next();
	}

	return 0;
}
