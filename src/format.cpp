#include "../inc/format"

// if meet '{' or ';', print std::endl
bool Format::formatProgramme(Lexer &lex) {
	int indent = 0;

	while (lex.get_type() != tokenEOF) {
		if (lex.get_type() == punctRBrace) {
			indent --;
			for (int i = 0; i < indent; i ++) 
				stream << "\t";
			stream << lex.get_token() << std::endl;
			lex.next();
			continue;
		}
		for (int i = 0; i < indent; i ++) 
			stream << "\t";
		while (lex.get_type() != punctSemicolon 
			&& lex.get_type() != punctLBrace)
		{
			stream << lex.get_token() << " ";
			lex.next();
		}
		if (lex.get_type() == punctLBrace) 
			indent ++;
		stream << lex.get_token() << std::endl;
		lex.next();
	}

	return true;
}
