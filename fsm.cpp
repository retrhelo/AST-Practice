#include <stdexcept>

#include <cctype>

#include "fsm"

fsm::fsm(std::istream &strm): 
	stat(IDLE), istrm(strm) {
	if (istrm.good()) 
		;
	else 
		throw std::runtime_error("fsm::fsm(): stream in bad status\n");
}

inline static bool _match_type_specifier(std::string const &str) {
	return str == "char" ||
		str == "short" ||
		str == "unsigned short" ||
		str == "signed short" ||
		str == "int" ||
		str == "unsigned int" ||
		str == "signed int" ||
		str == "long" ||
		str == "unsigned long" ||
		str == "signed long" ||
		str == "long long" ||
		str == "float" ||
		str == "double" ||
		str == "void";
}

inline static bool _match_storage_type_specifier(std::string const &str) {
	return str == "const" || 
		str == "auto" ||
		str == "static" ||
		str == "volatile";
}

inline static bool _match_operator(std::string const &str) {
	return str == "&" ||
		str == "~" ||
		str == "!" ||
		str == "+" ||
		str == "-" ||
		str == "*" ||
		str == "/" ||
		str == "%" ||
		str == "=" ||
		str == "==" ||
		str == "<" ||
		str == "<=" ||
		str == ">" ||
		str == ">=" ||
		str == "+=" ||
		str == "-=" ||
		str == "*=" ||
		str == "/=";
}

bool fsm::update(void) {
	bool flag = true;
	char ch;
	stat = IDLE;
	buf.clear();

	while (flag) {
		if ((ch = istrm.get()) == EOF) 
			break;
		switch (stat) {
			case IDLE: 
				if (isspace(ch)) 
					;
				else {
					buf.push_back(ch);
					if (isalpha(ch) || ch == '_') 
						stat = IDENTIFIER;
					else if (ch == '0') 
						stat = OCT_INTEGER;
					else if (isdigit(ch)) 
						stat = DEC_INTEGER;
					else if (_match_operator(buf)) 
						stat = OPERATOR;
					else {
						switch (ch) {
							case '\'': stat = CHARACTER; break;
							case '"': stat = CONST_STRING; break;
							case ',': stat = COMMA; break;
							case ';': stat = SEMI; break;
							case '{': stat = BRACES_LEFT; break;
							case '}': stat = BRACES_RIGHT; break;
							case '(': stat = PARENTHESES_LEFT; break;
							case ')': stat = PARENTHESES_RIGHT; break;
							case '[': stat = BRACKETS_LEFT; break;
							case ']': stat = BRACKETS_RIGHT; break;
							case '#': stat = PREPROCESSOR; break;
							default: 
								stat = ERROR;
						}
					}
				}
			break;
			case IDENTIFIER: // case identifier deals with all alphabet-related situation
				if (isalpha(ch) || isdigit(ch) || ch == '_') {
					buf.push_back(ch);
					if (_match_type_specifier(buf)) 
						stat = TYPE_SPECIFIER;
					else if (_match_storage_type_specifier(buf)) 
						stat = STORAGE_TYPE_SPECIFIER;
					else if (buf == "return") stat = RETURN;
					else if (buf == "if") stat = IF;
					else if (buf == "else") stat = ELSE;
					else if (buf == "while") stat = WHILE;
					else if (buf == "do") stat = DO;
				}
				else {
					istrm.unget();
					flag = false;
				}
				break;
			case CHARACTER: // read the whole character
				buf.push_back(ch);
				if (ch == '\\') {	// if is an escaping character
					buf.push_back(ch = istrm.get());
				}
				buf.push_back(ch = istrm.get());
				if (ch != '\'') 
					stat = ERROR;
				else 
					flag = false;
				break;
			case CONST_STRING: 
				buf.push_back(ch);
				while ((ch = istrm.get()) != '"' && ch != EOF) 
					buf.push_back(ch);
				if (ch == EOF) 
					stat = ERROR;
				else {
					buf.push_back(ch);
					flag = false;
				}
				break;
			case DEC_INTEGER: 
				if (isdigit(ch)) 
					buf.push_back(ch);
				else if (ch == 'f' || ch == 'F') {
					buf.push_back(ch);
					stat = REAL_NUMBER;
					flag = false;
				}
				else if (ch == 'l' || ch == 'L' ||
				  ch == 'u' || ch == 'U') {
					buf.push_back(ch);
					if ((ch = istrm.get()) == 'l' || ch == 'L') 
						buf.push_back(ch);
					else 
						istrm.unget();
					flag = false;
				}
				else if (ch == '.') {
					buf.push_back(ch);
					stat = REAL_NUMBER;
				}
				else {
					flag = false;
					istrm.unget();
				}
				break;
			case OCT_INTEGER: 
				if (ch >= '0' && ch <= '7') 
					buf.push_back(ch);
				else if (buf == "0" && (ch == 'x' || ch == 'X')) {
					buf.push_back(ch);
					stat = HEX_INTEGER;
				} 
				else {
					flag = false;
					istrm.unget();
				}
				break;
			case HEX_INTEGER: 
				if (isdigit(ch) || (tolower(ch) >= 'a' && tolower(ch) <= 'f')) 
					buf.push_back(ch);
				else {
					flag = false;
					istrm.unget();
				}
				break;
			case REAL_NUMBER: 
				if (isdigit(ch)) 
					buf.push_back(ch);
				else {
					flag = false;
					istrm.unget();
				}
				break;
			case OPERATOR: 
				buf.push_back(ch);
				if (_match_operator(buf)) 
					;
				else if (buf == "/*") 
					stat = BLOCK_COMMENT;
				else if (buf == "//") 
					stat = LINE_COMMENT;
				else {
					buf.pop_back();
					istrm.unget();
					flag = false;
				}
				break;
			case TYPE_SPECIFIER: 
			case STORAGE_TYPE_SPECIFIER: 
			case RETURN: 
			case IF: 
			case ELSE: 
			case WHILE: 
			case DO: 
				if (isalpha(ch) || isdigit(ch) || ch == '_') {
					stat = IDENTIFIER;
					buf.push_back(ch);
				}
				else {
					flag = false;
					istrm.unget();
				}
				break;
			case COMMA: 
			case SEMI: 
			case BRACES_LEFT: 
			case BRACES_RIGHT: 
			case PARENTHESES_LEFT: 
			case PARENTHESES_RIGHT: 
			case BRACKETS_LEFT: 
			case BRACKETS_RIGHT: 
				istrm.unget();
				flag = false;
				break;
			case PREPROCESSOR: 
				buf.push_back(ch);
				while ((ch = istrm.get()) != '\n') {
					buf.push_back(ch);
					if (ch == '\\') {
						ch = istrm.get();
						if (ch == '\n') 
							buf.pop_back();
						else 
							buf.push_back(ch);
					}
				}
				flag = false;
				break;
			case BLOCK_COMMENT: 
				while (1) {	// get chars until it matches "*/"
					if (ch == EOF) {
						stat = ERROR;
						break;
					}
					buf.push_back(ch);
					if (ch == '*') {
						if ((ch = istrm.get()) == '/') 
							break;
						buf.push_back(ch);
					}
					ch = istrm.get();
				}
				break;
			case LINE_COMMENT: 
				while (ch != '\n') {
					buf.push_back(ch);
					ch = istrm.get();
				}
			case ERROR: // error handling
				throw std::runtime_error("fsm::update(): invalid token '" + buf + "'");
			default: 
				throw std::runtime_error("fsm::update(): invalid stat value");
		}
	}

	/* 
		If flag != false, ch must be EOF. Thus we can say suppose 
		that we have reached the end of the stream. 
	*/
	return flag == false;
}
