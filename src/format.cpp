#include <deque>
#include <string>

#include "../inc/format"

#pragma GCC dependence "../inc/format"

static std::deque<std::string> s;

static void enterDebug(std::string str) {
	s.push_back(str);
}

static void debug_msg(std::string str) {
	std::cerr << s.back() << ": " 
		<< str << std::endl;
}

static void debug_msg(char *str) {
	std::cerr << s.back() << ": " 
		<< str << std::endl;
}

static void exitDebug(void) {
	s.pop_back();
}

#define debug_error() \
	do { \
		std::cerr << lex.get_line() << ":" << lex.get_lineChar() << ": "; \
		debug_msg("invalid token " + lex.get_token()); \
		exitDebug(); \
		return false; \
	} while (false)

class token_pair {
public: 
	token_t type;
	std::string token;

	token_pair(token_t tp, std::string tk): 
		type(tp), token(tk) {}
};

static std::deque<token_pair> queue;

static void tokenMatchNext(Lexer &lex) {
	queue.push_back(token_pair(lex.get_type(), lex.get_token()));

	std::cerr << "matched: " << 
		lex.get_line() << ":" << lex.get_lineChar() << ": "
		<< lex.get_token() << std::endl;
	lex.next();
}

static bool tokenTryMatchNext(Lexer &lex, token_t type) {
	if (type == lex.get_type()) {
		tokenMatchNext(lex);
		return true;
	}
	else return false;
}

static bool storageTypeTryMatchNext(Lexer &lex) {
	return tokenTryMatchNext(lex, keywordConst) 
		|| tokenTryMatchNext(lex, keywordAuto) 
		|| tokenTryMatchNext(lex, keywordStatic) 
		|| tokenTryMatchNext(lex, keywordVolatile) 
		|| tokenTryMatchNext(lex, keywordExtern);
}

static bool typeTryMatchNext(Lexer &lex) {
	bool ret =  tokenTryMatchNext(lex, keywordVoid) 
		||tokenTryMatchNext(lex, keywordChar) 
		|| tokenTryMatchNext(lex, keywordShort) 
		|| tokenTryMatchNext(lex, keywordInt) 
		|| tokenTryMatchNext(lex, keywordUnsigned) 
		|| tokenTryMatchNext(lex, keywordSigned) 
		|| tokenTryMatchNext(lex, keywordLong) 
		|| tokenTryMatchNext(lex, keywordFloat) 
		|| tokenTryMatchNext(lex, keywordDouble);

	if (!ret && tokenTryMatchNext(lex, keywordStruct) ||
		tokenTryMatchNext(lex, keywordUnion) ||
		tokenTryMatchNext(lex, keywordEnum)) 
	{
		ret = tokenTryMatchNext(lex, tokenIdent);
	}

	return ret;
}

bool Format::formatProgramme(Lexer &lex, int indent) {
	enterDebug("Programme");
	bool ret;

	while (lex.get_type() != tokenEOF) {
		// struct-union-def
		if (lex.get_type() == keywordStruct || 
			lex.get_type() == keywordUnion) 
		{
			stream << lex.get_token() << " ";
			lex.next();
			if (lex.get_type() != tokenIdent) 
				debug_error();
			stream << lex.get_token() << " ";
			
			lex.next();
			if (lex.get_type() != punctLBrace) 
				debug_error();
			stream << lex.get_token() << "\n";
			lex.next();

			while (lex.get_type() != punctRBrace) {
				for (int i = 0; i < indent + 1; i ++) 
					stream << '\t';
				while (true) {
					stream << lex.get_token();
					lex.next();
					if (lex.get_type() != punctSemicolon) 
						stream << " ";
					else {
						stream << lex.get_token() << std::endl;
						lex.next();
						break;
					}
				}
			}
			stream << lex.get_token();
			lex.next();
			if (lex.get_type() != punctSemicolon) 
				debug_error();
			else {
				stream << ";\n\n";
				lex.next();
			}
		}
		// enum-def
		else if (lex.get_type() == keywordEnum) {
			lex.next();
			stream << "enum " << lex.get_token();
			lex.next();
			if (lex.get_type() != punctLBrace) 
				debug_error();
			stream << " {\n";
			lex.next();
			while (lex.get_type() != punctRBrace) {
				for (int i = 0; i < indent + 1; i ++) 
					stream << '\t';
				while (true) {
					stream << lex.get_token();
					lex.next();
					if (lex.get_type() != punctComma) 
						stream << ' ';
					else {
						stream << ",\n";
						lex.next();
						break;
					}
				}
			}

			stream << lex.get_token();
			lex.next();
			if (lex.get_type() != punctSemicolon) 
				debug_error();
			else {
				stream << ";\n\n";
				lex.next();
			}
		}
		// var-def | func-def
		else if (storageTypeTryMatchNext(lex) 
			|| typeTryMatchNext(lex)) 
		{
			typeTryMatchNext(lex);

			while (lex.get_type() != punctSemicolon && 
				lex.get_type() != punctLBrace) 
			{
				tokenMatchNext(lex);
			}
			for (token_pair &it : queue) 
				stream << it.token << " ";
			queue.clear();
			if (lex.get_type() == punctLBrace) {
				stream << lex.get_token() << std::endl;
				lex.next();
				ret = formatComplex(lex, indent + 1);
				stream << "\n\n";
			}
			else {
				stream << lex.get_token() << std::endl;
				lex.next();
				ret = true;
			}
		}
		else debug_error();
	}

	exitDebug();
	return ret;
}

/* pre-print: 
	"{"
*/
bool Format::formatComplex(Lexer &lex, int indent) {
	enterDebug("Complex");

	bool ret = true;

	while (lex.get_type() != punctRBrace) {
		for (int i = 0; i < indent; i ++) 
			stream << '\t';
		switch (lex.get_type()) {
		case keywordIf: 
			ret = formatIf(lex, indent + 1);
			break;
		case keywordWhile: 
			ret = formatWhile(lex, indent + 1);
			break;
		case keywordDo: 
			ret = formatDoWhile(lex, indent + 1);
			break;
		case keywordFor: 
			ret = formatFor(lex, indent + 1);
			break;
		default: 
			while (true) {
				stream << lex.get_token();
				lex.next();
				if (lex.get_type() != punctSemicolon) 
					stream << ' ';
				else {
					stream << ";\n";
					lex.next();
					break;
				}
			}
			break;
		}
	}
	for (int i = 0; i < indent - 1; i ++) 
		stream << '\t';
	stream << '}';
	lex.next();

	if (false == ret) debug_error();

	exitDebug();
	return ret;
}

bool Format::formatIf(Lexer &lex, int indent) {
	enterDebug("If");

	bool ret = true;

	stream << "if (";
	lex.next(); lex.next();

	int cnt = 1;
	while (cnt > 0) {
		if (lex.get_type() == punctLParen) cnt ++;
		else if (lex.get_type() == punctRParen) cnt --;
		stream << lex.get_token();
		lex.next();
	}

	if (lex.get_type() == punctLBrace) {
		stream << " {\n";
		lex.next();
		ret = formatComplex(lex, indent);
		stream << std::endl;
	}
	else {
		stream << std::endl;
		ret = formatExpr(lex, indent);
	}

	if (lex.get_type() == keywordElse) {
		for (int i = 0; i < indent - 1; i ++) 
			stream << '\t';
		stream << "else ";
		lex.next();
		if (lex.get_type() == punctLBrace) {
			stream << "{\n";
			lex.next();
			ret = formatComplex(lex, indent);
			stream << std::endl;
		}
		else if (lex.get_type() == keywordIf) 
			ret = formatIf(lex, indent);
		else {
			stream << std::endl;
			ret = formatExpr(lex, indent);
		}
	}

	if (false == ret) 
		debug_error();
	
	exitDebug();
	return ret;
}

bool Format::formatWhile(Lexer &lex, int indent) {
	enterDebug("While");

	bool ret = true;

	stream << "while (";
	lex.next(); lex.next();

	int cnt = 1;
	while (cnt > 0) {
		if (lex.get_type() == punctLParen) cnt ++;
		else if (lex.get_type() == punctRParen) cnt --;
		stream << lex.get_token();
		lex.next();
	}

	if (lex.get_type() == punctLBrace) {
		stream << " {\n";
		lex.next();
		ret = formatComplex(lex, indent);
		stream << std::endl;
	}
	else {
		stream << std::endl;
		ret = formatExpr(lex, indent);
	}

	if (false == ret) 
		debug_error();
	
	exitDebug();
	return ret;
}

bool Format::formatDoWhile(Lexer &lex, int indent) {
	enterDebug("DoWhile");

	stream << "do {";
	lex.next(); lex.next();

	if (lex.get_type() == punctRBrace) {
		stream << "} while (";
		lex.next();lex.next();lex.next();
	}
	else {
		stream << std::endl;
		formatComplex(lex, indent);
		stream << " while (";
		lex.next(); lex.next();
	}

	while (lex.get_type() != punctSemicolon) {
		stream << lex.get_token();
		lex.next();
	}
	stream << ";\n";
	lex.next();

	exitDebug();
	return true;
}

bool Format::formatFor(Lexer &lex, int indent) {
	enterDebug("For");

	stream << "for (";
	lex.next(); lex.next();

	while (true) {
		stream << lex.get_token();
		lex.next();
		if (lex.get_type() != punctSemicolon) 
			stream << ' ';
		else {
			stream << "; ";
			lex.next();
			break;
		}
	}
	while (true) {
		stream << lex.get_token();
		lex.next();
		if (lex.get_type() != punctSemicolon) 
			stream << ' ';
		else {
			stream << "; ";
			lex.next();
			break;
		}
	}
	while (true) {
		stream << lex.get_token();
		lex.next();
		if (lex.get_type() != punctRParen) 
			stream << ' ';
		else {
			stream << ") ";
			lex.next();
			break;
		}
	}

	if (lex.get_type() == punctLBrace) {
		stream << "{\n";
		lex.next();
		formatComplex(lex, indent);
		stream << std::endl;
	}
	else {
		stream << std::endl;
		formatExpr(lex, indent);
	}

	exitDebug();
	return true;
}

bool Format::formatExpr(Lexer &lex, int indent) {
	enterDebug("Expression");

	for (int i = 0; i < indent; i ++) 
		stream << '\t';
	while (true) {
		stream << lex.get_token();
		lex.next();
		if (lex.get_type() != punctSemicolon) 
			stream << ' ';
		else {
			stream << ";\n";
			lex.next();
			break;
		}
	}

	exitDebug();
	return true;
}
