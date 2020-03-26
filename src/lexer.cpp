#include <cctype>

#include "../lexer"

#pragma GCC dependency ../lexer

token_t Lexer::match_keyword(std::string &str) {
	if (str == "if") 
		return keywordIf;
	else if (str == "else") 
		return keywordElse; 
	else if (str == "while") 
		return keywordWhile;
	else if (str == "do") 
		return keywordDo;
	else if (str == "for") 
		return keywordFor;
	else if (str == "sizeof") 
		return keywordSizeof;
	else if (str == "const") 
		return keywordConst;
	else if (str == "auto") 
		return keywordAuto;
	else if (str == "static") 
		return keywordStatic;
	else if (str == "extern") 
		return keywordExtern;
	else if (str == "typedef") 
		return keywordTypedef;
	else if (str == "struct") 
		return keywordStruct;
	else if (str == "union") 
		return keywordUnion;
	else if (str == "enum") 
		return keywordEnum;
	else if (str == "void") 
		return keywordVoid;
	else if (str == "bool") 
		return keywordBool;
	else if (str == "char") 
		return keywordChar;
	else if (str == "int") 
		return keywordInt;
	else if (str == "unsigned") 
		return keywordUnsigned;
	else if (str == "long") 
		return keywordLong;
	else if (str == "short") 
		return keywordShort;
	else if (str == "double") 
		return keywordDouble;
	else if (str == "float") 
		return keyworFloat;
	else if (str == "true") 
		return keywordTrue;
	else if (str == "false") 
		return keywordFalse;
	else 
		return tokenUndefined;
}

token_t Lexer::match_punct(void) {
	eatCharNext();
	switch (token[0]) {
		case '{': return punctLBrace;
		case '}': return punctRBrace;
		case '(': return punctLParen;
		case ')': return punctRParen;
		case '[': return punctLBracket;
		case ']': return punctRBracket;
		case ';': return punctSemicolon;
		case ',': return punctComma;
		case '=': /* == or = */
			return eatTryNext('=') ? punctEqual : punctAssign;
		case '!': /* ! or != */
			return eatTryNext('=') ? punctNotEqual : punctLogicalNot;
		case '>': /* > or >= or >> or >>= */
			if (eatTryNext('=')) 
				return punctGreaterEqual;
			else if (eatTryNext('>')) 
				return eatTryNext('=') ? punctShrAssign : punctShr;
			else 
				return punctGreater;
		case '<': /* < or <= or << or <<= */
			if (eatTryNext('=')) 
				return punctLessEqual;
			else if (eatTryNext('<')) 
				return eatTryNext('=') ? punctShlAssign : punctShl;
			else 
				return punctLess;
		case '?': return punctQuestion;
		case ':': return punctColon;
		case '&': /* & or && or &= */
			if (eatTryNext('=')) 
				return punctBitwiseAndAssign;
			else if (eatTryNext('&')) 
				return punctLogicalAnd;
			else 
				return punctBitwiseAnd;
		case '|': /* | or || or |= */
			if (eatTryNext('=')) 
				return punctBitwiseOrAssign;
			else if (eatTryNext('|')) 
				return punctLogicalOr;
			else 
				return punctBitwiseOr;
			case '^': /* ^ or ^= */
				return eatTryNext('=') ? punctBitwiseXorAssign : 
					punctBitwiseXor;
			case '~': return punctBitwiseNot;
			case '+': /* + or ++ or += */
				if (eatTryNext('=')) 
					return punctPlusAssign;
				else if (eatTryNext('+')) 
					return punctPlusPlus;
				else 
					return punctPlus;
			case '-': /* - or -- or -= */
				if (eatTryNext('=')) 
					return punctMinusAssign;
				else if (eatTryNext('-')) 
					return punctMinusMinus;
				else 
					return punctMinus;
			case '*': /* * or *= */
				return eatTryNext('=') ? punctTimesAssign : punctTimes;
			case '/': /* / or /= */
				return eatTryNext('=') ? punctDivideAssign : punctDivide;
			case '%': /* % or %= */
				return eatTryNext('=') ? punctModuloAssign : punctModulo;
			case EOF: 
				return tokenEOF;
			dafault: /* unknown punctuation */
				return tokenUndefined;
	}

	return tokenUndefined;
}

void Lexer::next(void) {
	/* escape space characters */
	while (isspace(cur)) 
		nextChar();

	// save location
	token_line = cur_line;
	token_lineChar = cur_lineChar;
	token_pos = stream.tellg();

	token = "";
	type = tokenUndefined;

	/* end of file */
	if (cur == EOF) 
		type = tokenEOF;
	/* preprocessor */
	else if (cur == '#') {
		type = tokenPreprocess;
		while (cur != '\n') {
			if (cur == '\\') 		// considering '\\' for multiple lines
				nextChar();
			else 
				eatCharNext();
		}
	}
	/* comments */
	else if (cur == '\\') {
		type = tokenComment;
		if (eatTryNext('\\')) {
			while (cur != '\n') 
				eatCharNext();
		}
		else if (eatTryNext('*')) {
			while (true) {
				// use short-curcuit rule
				if (eatTryNext('*') && eatTryNext('\\')) 
					break;
				eatCharNext();
			}
		}
		else 
			type = tokenUndefined;
	}
	/* Ident or keyword */
	else if (isalpha(cur) || cur == '_') {
		eatCharNext();
		while (isalnum(cur) || cur == '_') 
			eatCharNext();

		type = match_keyword(token);
		if (type == tokenUndefined) 
			type = tokenIdent;
	}
	/* integer and double */
	else if (isdigit(cur)) {
		type = tokenIdent;
		eatCharNext();
		if (eatTryNext('x') || eatTryNext('X')) {
			while (isalnum(cur)) 
				eatCharNext();
		}
		else while (isdigit(cur) || cur == '.') {
			if (cur == '.') 
				type = tokenDouble;
			eatCharNext();
		}
		// deal with suffix
		while (cur == 'u' || cur == 'U' ||
			cur == 'l' || cur == 'L') 
			eatCharNext();
	}
	/* string or character */
	else if (cur == '"' || cur == '\'') {
		type = (cur == '"' ? tokenStr : tokenChar);
		eatCharNext();
		if (type == tokenStr) {
			while (cur != '"') {
				if (cur == '\\') 
					nextChar();
				else 
					eatCharNext();
			}
		}
		else {
			eatTryNext('\\');
			eatCharNext();
			if (!eatTryNext('\'')) 
				type = tokenUndefined;
		}
		eatCharNext();
	}
	/* punctuation or a unknown character */
	else {
		type = match_punct();
	}
}

char Lexer::nextChar(void) {
	cur = stream.get();

	if (cur == '\n') {
		cur_line ++;
		cur_lineChar = 0;
	}
	else if (cur != EOF) 
		cur_lineChar ++;

	return cur;
}

void Lexer::eatCharNext(void) {
	token.push_back(cur);
	nextChar();
}

bool Lexer::eatTryNext(char ch) {
	if (ch == cur) {
		token.push_back(cur);
		nextChar();
		return true;
	}
	
	return false;
}
