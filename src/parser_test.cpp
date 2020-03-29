#include <iostream>
#include <fstream>
#include <string>

#include "../inc/lexer"
#include "../inc/parser"

int main(void) {
	std::string filename;
	
	std::cerr << "filename: ";
	std::cin >> filename;

	std::ifstream ifstrm(filename);
	Lexer lex(ifstrm);
	lex.next();
	Parser p(std::cout);
	p.parserProgramme(lex);

	return 0;
}
