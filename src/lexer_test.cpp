#include <iostream>
#include <string>
#include <fstream>

#include "../inc/lexer"

int main(void) {
	std::string filename;

	std::cout << "filename: ";
	std::cin >> filename;

	std::ifstream ifstrm(filename);
	Lexer lex(ifstrm);

	lex.next();
	while (lex.get_type() != tokenEOF) {
		std::cout << lex.get_line() << ":" << lex.get_lineChar() 
			<< ": "  << lex.get_type() << ": "
			<< lex.get_token() << std::endl;
		lex.next();
	}

	return 0;
}
