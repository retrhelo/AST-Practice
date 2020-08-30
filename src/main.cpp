#include <fstream>
#include <iostream>
#include <cstring>

#include "../inc/parser"
#include "../inc/lexer"
#include "../inc/format"

#define STR_EQU(pstr1, pstr2) \
	!strcmp(pstr1, pstr2)

int main(int argc, char *argv[]) {
	if (1 == argc) {	// display usage
		std::cout << "Usage: " << argv[0] << "\n"
			<< " -f in_file" << "\t\tassign input file\n" 
			<< " -o out_file" << "\t\tassign output file\n" 
			<< " --format" << "\t\tmode = format\n" 
			<< " --ast" << "\t\tmode = parser\n" 
			<< " -h" << "\t\tdisplay this page" << std::endl;
		return 0;
	}

	std::string infile = "";
	std::string outfile = "";
	bool mode = true;		// mode = true, parser; mode = false, format

	for (int i = 1; i < argc; i ++) {
		if (STR_EQU(argv[i], "-f")) 
			infile = argv[++i];
		else if (STR_EQU(argv[i], "-o")) 
			outfile = argv[++i];
		else if (STR_EQU(argv[i], "--format")) 
			mode = false;
		else if (STR_EQU(argv[i], "--ast")) 
			mode = true;
		else if (STR_EQU(argv[i], "-h")) {
			std::cout << "Usage: " << argv[0] << "\n"
				<< " -f in_file" << "\t\tassign input file\n" 
				<< " -o out_file" << "\t\tassign output file\n" 
				<< " --format" << "\t\tmode = format\n" 
				<< " --ast" << "\t\tmode = parser\n" 
				<< " -h" << "\t\tdisplay this page" << std::endl;
			return 0;
		}
		else {
			std::cerr << "invalid flag: " << argv[i] 
				<< std::endl;
			return -1;
		}
	}

	std::ifstream istrm(infile);
	if (!istrm.is_open()) {
		std::cerr << "warning: invalid input file" 
			<< std::endl;
		return -1;
	}

	Lexer lex(istrm);
	lex.next();
	std::ofstream ostrm(outfile);
	if (ostrm.is_open()) {
		if (mode) {
			Parser p(ostrm);
			p.parserProgramme(lex);
		}
		else {
			Format f(ostrm);
			f.formatProgramme(lex);
		}
	}
	else {
		if (mode) {
			Parser p(ostrm);
			p.parserProgramme(lex);
		}
		else {
			Format f(ostrm);
			f.formatProgramme(lex);
		}
	}

	istrm.close();
	ostrm.close();

	return 0;
}
