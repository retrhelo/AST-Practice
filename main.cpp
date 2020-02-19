#include <iostream>
#include <fstream>
#include <string>
#include <exception>

#include "fsm"

int main(void) {
	std::string filename;

	std::cout << "filename: ";
	std::cin >> filename;

	std::ifstream ifstrm(filename);
	if (!ifstrm.is_open()) 
		return -1;

	try {
		fsm sm(ifstrm);
		while (sm.update()) {
			std::cout << "token: " << sm.get_token();
			std::cout << " type: ";
			switch (sm.get_token_type()) {
				case fsm::TYPE_SPECIFIER: 
					std::cout << "type specifier";
					break;
				case fsm::STORAGE_TYPE_SPECIFIER: 
					std::cout << "storage type specifier";
					break;
				case fsm::IDENTIFIER: 
					std::cout << "identifier";
					break;
				case fsm::CHARACTER: 
					std::cout << "character";
					break;
				case fsm::CONST_STRING: 
					std::cout << "const string";
					break;
				case fsm::DEC_INTEGER: 
				case fsm::OCT_INTEGER: 
				case fsm::HEX_INTEGER: 
					std::cout << "integer";
					break;
				case fsm::REAL_NUMBER: 
					std::cout << "real number";
					break;
				case fsm::OPERATOR: 
					std::cout << "operator";
					break;
				case fsm::RETURN: 
					std::cout << "return";
					break;
				case fsm::IF: 
					std::cout << "if";
					break;
				case fsm::ELSE: 
					std::cout << "else";
					break;
				case fsm::WHILE: 
					std::cout << "while";
					break;
				case fsm::DO: 
					std::cout << "do";
					break;
				case fsm::COMMA: 
					std::cout << "comma";
					break;
				case fsm::SEMI: 
					std::cout << "semicomma";
					break;
				case fsm::BRACES_LEFT: 
					std::cout << "left braces";
					break;
				case fsm::BRACES_RIGHT: 
					std::cout << "right braces";
					break;
				case fsm::PARENTHESES_LEFT: 
					std::cout << "left parentheses";
					break;
				case fsm::PARENTHESES_RIGHT: 
					std::cout << "right parentheses";
					break;
				case fsm::BRACKETS_LEFT: 
					std::cout << "left brackets";
					break;
				case fsm::BRACKETS_RIGHT: 
					std::cout << "right brackets";
					break;
				case fsm::PREPROCESSOR: 
					std::cout << "preprocessor";
					break;
				case fsm::BLOCK_COMMENT: 
				case fsm::LINE_COMMENT: 
					std::cout << "comment";
					break;
				default: 
					std::cout << "unknown type";
			}
			std::cout << std::endl;
		}
	} catch(std::exception &e) {
		std::cout << e.what();
	}

	return 0;
}
