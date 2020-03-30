#include <iostream>
#include <fstream>
#include <string>

#include "../inc/lexer"
#include "../inc/format"

int main(void) {
	std::string filename;

	std::cout << "filename: ";
	std::cin >> filename;

	std::ifstream in(filename);
	std::ofstream out("output");
	Lexer lex(in);
	Format format(out);

	lex.next();
	format.formatProgramme(lex);

	return 0;
}
