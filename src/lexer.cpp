#include <cctype>

#include "../inc/lexer"

#pragma dependence "../inc/lexer"

char Lexer::charNext(void) {
	if (cur_char == '\n') {
		cur_line ++;
		cur_lineChar = 1;
	}
	else if (cur_char != EOF) 
		cur_lineChar ++;

	return cur_char = stream.get();
}

char Lexer::eatNext(void) {
	char ret = cur_char;

	token.push_back(ret);
	charNext();

	return ret;
}

bool Lexer::eatTryNext(char ch) {
	if (cur_char == ch) {
		eatNext();
		return true;
	}
	else 
		return false;
}

static token_t keywordMatch(std::string &str) {
	if ("if" == str) return keywordIf;
	else if ("else" == str) return keywordElse; 
	else if ("while" == str) return keywordWhile;
	else if ("do" == str) return keywordDo;
	else if ("for" == str) return keywordFor; 
	else if ("return" == str) return keywordReturn;
	else if ("break" == str) return keywordBreak; 
	else if ("continue" == str) return keywordContinue; 
	else if ("sizeof" == str) return keywordSizeof;
	else if ("const" == str) return keywordConst; 
	else if ("auto" == str) return keywordAuto;
	else if ("static" == str) return keywordStatic;
	else if ("volatile" == str) return keywordVolatile;
	else if ("extern" == str) return keywordExtern; 
	else if ("void" == str) return keywordVoid;
	else if ("char" == str) return keywordChar;
	else if ("short" == str) return keywordShort;
	else if ("int" == str) return keywordInt; 
	else if ("unsigned" == str) return keywordUnsigned;
	else if ("signed" == str) return keywordSigned;
	else if ("long" == str) return keywordLong;
	else if ("float" == str) return keywordFloat;
	else if ("double" == str) return keywordDouble;
	else if ("struct" == str) return keywordStruct;
	else if ("union" == str) return keywordUnion;
	else if ("enum" == str) return keywordEnum;
	else 
		return tokenUndefined;
}

token_t Lexer::lexerPunct(void) {
	switch (token[0]) {
		case '{': return punctLBrace;
		case '}': return punctRBrace;
		case '(': return punctLParen;
		case ')': return punctRParen;
		case '[': return punctLBracket;
		case ']': return punctRBracket;
		case ';': return punctSemicolon;
		case '.': return punctPeriod;
		case ',': return punctComma;
		case '=': 
			return eatTryNext('=') ? punctEqual : punctAssign;
		case '!': 
			return eatTryNext('=') ? punctNotEqual : punctLogicalNot;
		case '>': return eatTryNext('=') ? punctGreaterEqual : 
					eatTryNext('>') ? 
					(eatTryNext('=') ? punctShrAssign : punctShr)
					: punctGreater;
		case '<': return eatTryNext('=') ? punctLessEqual
						: eatTryNext('<') ? (eatTryNext('=') ? punctShlAssign : punctShl)
						: punctLess;

		case '?': return punctQuestion;
		case ':': return punctColon;

		case '&': return eatTryNext('=') ? punctBitwiseAndAssign
						: eatTryNext('&') ? punctLogicalAnd 
						: punctBitwiseAnd;
		case '|': return eatTryNext('=') ? punctBitwiseOrAssign
						: eatTryNext('|') ? punctLogicalOr 
						: punctBitwiseOr;
		case '^': return eatTryNext('=') ? punctBitwiseXorAssign : punctBitwiseXor;
		case '~': return punctBitwiseNot;

		case '+': return eatTryNext('=') ? punctPlusAssign
						: eatTryNext('+') ? punctPlusPlus : punctPlus;
		case '-': return eatTryNext('=') ? punctMinusAssign
						: eatTryNext('-') ? punctMinusMinus
						: eatTryNext('>') ? punctArrow 
						: punctMinus;
		case '*': return eatTryNext('=') ? punctTimesAssign : punctTimes;
		case '/': return eatTryNext('=') ? punctDivideAssign : punctDivide;
		case '%': return eatTryNext('=') ? punctModuloAssign : punctModulo;
		default: return tokenUndefined;
	}
}

void Lexer::next(void) {
	if (type == tokenEOF) 
		return ;

	// escape insignificants
	while (true) {
		if (isspace(cur_char)) 
			charNext();
		else {
			token = "";
			token_line = cur_line;
			token_lineChar = cur_lineChar;
			// preprocessor
			if (eatTryNext('#')) {
				while (cur_char != '\n' && cur_char != EOF) 
					eatNext();
			}
			// comments
			else if (eatTryNext('/')) {
				if (eatTryNext('/')) {
					while (cur_char != '\n' && cur_char != EOF) 
						eatNext();
				}
				else if (eatTryNext('*')) {
					while (true) {
						if (eatTryNext('*') && eatTryNext('/')) 
							break;
						else 
							eatNext();
					}
				}
				else {
					eatNext();
					break;
				}
			}
			// find something important
			else {
				eatNext();
				break;
			}
			std::cerr << "match:" << token_line << ":" 
				<< token_lineChar << ": " << token << std::endl;
		}
	}

	if (token[0] == EOF) 
		type = tokenEOF;
	// Ident or keyword
	else if (isalpha(token[0]) || token[0] == '_') {
		while (isalnum(cur_char) || cur_char == '_') 
			eatNext();
		type = keywordMatch(token);
		if (type == tokenUndefined) 
			type = tokenIdent;
	}
	// integer or double
	else if (isdigit(token[0])) {
		type = tokenInt;
		while (cur_char != EOF) {
			if (isdigit(cur_char)) 
				eatNext();
			else if (token.length() == 1 && token[0] == '0' && tolower(cur_char) == 'x') {
				eatNext();
				while (isalnum(cur_char)) eatNext();
				break;
			}
			else if (cur_char == '.' && type == tokenInt) {
				eatNext();
				type = tokenDouble;
			}
			else break;
		}
		// suffix
		while (cur_char == 'f' || cur_char == 'l' || cur_char == 'L' || 
			cur_char == 'u' || cur_char == 'U') 
			eatNext();
	}
	// string
	else if (token[0] == '"') {
		type = tokenStr;
		while (!eatTryNext('"')) {
			if (cur_char == EOF) {	// end without '"'
				type = tokenUndefined;
				break;
			}
			eatTryNext('\\');
			eatNext();
		}
	}
	// character
	else if (token[0] == '\'') {
		type = tokenChar;
		eatTryNext('\\');
		eatNext();
		if (!eatTryNext('\'')) 
			type = tokenUndefined;
	}
	// punctuation or an unrecognised character
	else {
		type = lexerPunct();
	}
}
