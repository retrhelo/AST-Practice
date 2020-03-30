#include <deque>
#include <string>
#include <iostream>

#include "../inc/parser"

#pragma GCC dependence "../inc/parser"

std::deque<std::string> s;

void enterDebug(std::string str) {
	s.push_back(str);
}

void debug_msg(std::string str) {
	std::cerr << s.back() << ": " 
		<< str << std::endl;
}

void debug_msg(char *str) {
	std::cerr << s.back() << ": " 
		<< str << std::endl;
}

void exitDebug(void) {
	s.pop_back();
}

static std::string decoder(token_t type) {
	if (type == tokenUndefined) return "Undefined";
	else if (type == tokenEOF) return "EOF";
	else if (type == tokenIdent) return "Identifier";
	else if (type == tokenInt) return "Integer";
	else if (type == tokenDouble) return "Float number";
	else if (type == tokenChar) return "Character";
	else if (type == tokenStr) return "String";
	else if (type >= keywordIf && type <= keywordEnum) 
		return "Keyword";
	else return "punctuation";
}

class token_pair {
public:
	token_t type;
	std::string token;

	token_pair(token_t tp, std::string tk): 
		type(tp), token(tk) {}
};

std::deque<token_pair> queue;

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
	return tokenTryMatchNext(lex, keywordVoid) 
		||tokenTryMatchNext(lex, keywordChar) 
		|| tokenTryMatchNext(lex, keywordShort) 
		|| tokenTryMatchNext(lex, keywordInt) 
		|| tokenTryMatchNext(lex, keywordUnsigned) 
		|| tokenTryMatchNext(lex, keywordSigned) 
		|| tokenTryMatchNext(lex, keywordLong) 
		|| tokenTryMatchNext(lex, keywordFloat) 
		|| tokenTryMatchNext(lex, keywordDouble) 
		|| tokenTryMatchNext(lex, keywordStruct) 
		|| tokenTryMatchNext(lex, keywordUnion) 
		|| tokenTryMatchNext(lex, keywordEnum);
}

void Parser::println_indent(std::string str, int indent) {
	while (indent --) 
		stream << '\t';
	stream << str << std::endl;
}

void Parser::queue_clean(int indent) {
	for (token_pair &it : queue) 
		println_indent(decoder(it.type) + ": " + it.token, indent);
	queue.clear();
}

// exception handling
#define debug_error() \
	do {\
		debug_msg("invalid token " + lex.get_token()); \
		exitDebug(); \
		return false; \
	} while (false) 

bool Parser::parserProgramme(Lexer &lex, int indent) {
	enterDebug("Programme");
	bool ret;
	
	println_indent("Programme", indent);
	while (lex.get_type() != tokenEOF) {
		// struct-union-def
		if (tokenTryMatchNext(lex, keywordStruct) ||
			tokenTryMatchNext(lex, keywordUnion)) 
		{
			ret = parserStructUnionDef(lex, indent + 1);
		}
		// enum-def
		else if (tokenTryMatchNext(lex, keywordEnum)) 
			ret = parserEnumDef(lex, indent + 1);
		// var-def | func-def
		else if (storageTypeTryMatchNext(lex) 
			|| typeTryMatchNext(lex)) 
		{
			typeTryMatchNext(lex);
			// if pointer
			while (tokenTryMatchNext(lex, punctTimes)) 
				;
			if (!tokenTryMatchNext(lex, tokenIdent)) 
				debug_error();
			// func-def
			if (tokenTryMatchNext(lex, punctLParen)) 
				ret = parserFuncDef(lex, indent + 1);
			// var-def
			else 
				ret = parserVarDef(lex, indent + 1);
		}
		else 
			debug_error();
	}

	exitDebug();
	return ret;
}

/* pre-match: 
	[<storage-type>] <type> [{"*"}] <identifier> 
*/
bool Parser::parserVarDef(Lexer &lex, int indent) {
	enterDebug("VarDef");
	println_indent("VarDef", indent);

	queue_clean(indent);
	while (tokenTryMatchNext(lex, punctComma)) {
		if (!tokenTryMatchNext(lex, tokenIdent)) 
			debug_error();
	}
	if (!tokenTryMatchNext(lex, punctSemicolon)) 
		debug_error();
	queue_clean(indent);
	
	exitDebug();
	return true;
}

/* pre-matched: 
	"struct" | "union"
*/
bool Parser::parserStructUnionDef(Lexer &lex, int indent) {
	enterDebug("StructUnionDef");
	println_indent("Struct/Union Def", indent);

	queue_clean(indent);
	if (tokenTryMatchNext(lex, tokenIdent) && 
		tokenTryMatchNext(lex, punctLBrace)) 
	{
		// matching fields
		while (typeTryMatchNext(lex)) {
			if (tokenTryMatchNext(lex, tokenIdent) && 
				parserVarDef(lex, indent + 1)) 
			{}
			else debug_error();
		}

		if (tokenTryMatchNext(lex, punctRBrace) 
			&& tokenTryMatchNext(lex, punctSemicolon)) 
			;
		else debug_error();
	}
	else debug_error();
	queue_clean(indent);

	exitDebug();
	return true;
}

/* pre-matched: 
	"enum" 
*/
bool Parser::parserEnumDef(Lexer &lex, int indent) {
	enterDebug("EnumDef");
	println_indent("Enum Def", indent);

	queue_clean(indent);
	if (tokenTryMatchNext(lex, tokenIdent) && 
		tokenTryMatchNext(lex, punctLBrace)) 
	{
		while (tokenTryMatchNext(lex, tokenIdent)) {
			if (!tokenTryMatchNext(lex, punctComma)) 
				break;
		}
		for (token_pair &it : queue) 
			println_indent(decoder(it.type) + ": "  + it.token, indent + 1);
		queue.clear();

		if (tokenTryMatchNext(lex, punctRBrace) && 
			tokenTryMatchNext(lex, punctSemicolon)) 
			;
		else debug_error();
	}
	queue_clean(indent);

	exitDebug();
	return true;
}

/* pre-matched: 
	[<func-type>] <type> <identifier> "("
*/
bool Parser::parserFuncDef(Lexer &lex, int indent) {
	enterDebug("FuncDef");
	println_indent("Func Def", indent);

	queue_clean(indent);
	// "void" is a bit different from other types
	if (tokenTryMatchNext(lex, keywordVoid) && 
		tokenTryMatchNext(lex, punctRParen)) 
	{
		queue_clean(indent);
		if (tokenTryMatchNext(lex, punctSemicolon)) 
			queue_clean(indent);
		else if (parserComplex(lex, indent + 1))
			;
		else debug_error();
	}
	else if (parserArgList(lex, indent + 1) &&
		tokenTryMatchNext(lex, punctRParen)) 
	{
		queue_clean(indent);
		if (tokenTryMatchNext(lex, punctSemicolon)) 
			queue_clean(indent);
		else if (parserComplex(lex, indent + 1)) 
			;
		else debug_error();
	}
	else debug_error();

	exitDebug();
	return true;
}

/* pre-matched: 
*/
bool Parser::parserArgList(Lexer &lex, int indent) {
	enterDebug("ArgList");
	println_indent("Arg List", indent);

	if (parserArg(lex, indent + 1)) {
		while (tokenTryMatchNext(lex, punctComma)) {
			queue_clean(indent);
			if (!parserArg(lex, indent + 1)) 
				debug_error();
		}
	}
	else debug_error();

	exitDebug();
	return true;
}

/* pre-matched: 
*/
bool Parser::parserArg(Lexer &lex, int indent) {
	enterDebug("Arg");
	println_indent("Arg", indent);

	if (typeTryMatchNext(lex)) {
		// if pointer
		while (tokenTryMatchNext(lex, punctTimes)) 
			;
		if (!tokenTryMatchNext(lex, tokenIdent)) 
			debug_error();
		queue_clean(indent);
	}
	else debug_error();

	exitDebug();
	return true;
}

/* pre-matched: 
*/
bool Parser::parserComplex(Lexer &lex, int indent) {
	enterDebug("Complex");
	println_indent("Complex", indent);
	bool ret = true;

	if (!tokenTryMatchNext(lex, punctLBrace)) 
		debug_error();
	queue_clean(indent);
	while (ret) {
		if (tokenTryMatchNext(lex, punctRBrace)) {
			queue_clean(indent);
			break;
		}
		// <var-def>
		else if (storageTypeTryMatchNext(lex) ||
			typeTryMatchNext(lex)) 
		{
			typeTryMatchNext(lex);
			ret = tokenTryMatchNext(lex, tokenIdent) && 
				parserVarDef(lex, indent + 1);
		}
		// <if-block>
		else if (tokenTryMatchNext(lex, keywordIf)) 
			ret = parserIf(lex, indent + 1);
		// <while-block>
		else if (tokenTryMatchNext(lex, keywordWhile)) 
			ret = parserWhile(lex, indent + 1);
		// <do-while-block>
		else if (tokenTryMatchNext(lex, keywordDo)) 
			ret = parserDoWhile(lex, indent + 1);
		// <for-block>
		else if (tokenTryMatchNext(lex, keywordFor)) 
			ret = parserFor(lex, indent + 1);
		// <jump>
		else if (tokenTryMatchNext(lex, keywordBreak) 
			|| tokenTryMatchNext(lex, keywordContinue) 
			|| tokenTryMatchNext(lex, keywordReturn)) 
			ret = parserJump(lex, indent + 1);
		// <expression> ";"
		else {
			ret = parserExpr(lex, indent + 1) && tokenTryMatchNext(lex, punctSemicolon);
			queue_clean(indent + 1);
		}

	}
	if (false == ret) 
		debug_error();

	exitDebug();
	return true;
}

/* pre-matched: 
	"break" | "continue" | "return"
*/
bool Parser::parserJump(Lexer &lex, int indent) {
	enterDebug("Jump");
	println_indent("Jump", indent);

	queue_clean(indent);
	if (parserExpr(lex, indent + 1) && tokenTryMatchNext(lex, punctSemicolon)) 
		queue_clean(indent);
	else debug_error();

	exitDebug();
	return true;
}

/* pre-matched: 
	"if"
*/
bool Parser::parserIf(Lexer &lex, int indent) {
	enterDebug("If");
	println_indent("If", indent);

	if (tokenTryMatchNext(lex, punctLParen)) 
		queue_clean(indent);
	else debug_error();
	if (parserExpr(lex, indent + 1) && tokenTryMatchNext(lex, punctRParen)) {
		queue_clean(indent);
	}
	else debug_error();

	bool ret = true;
	if (lex.get_type() == punctLBrace) {
		ret = parserComplex(lex, indent + 1);
	}
	else {
		ret = parserExpr(lex, indent + 1) && tokenTryMatchNext(lex, punctSemicolon);
		queue_clean(indent);
	}

	if (false == ret) 
		debug_msg("invalid token " + lex.get_token());
	exitDebug();
	return ret;
}

/* pre-matched: 
	"while"
*/
bool Parser::parserWhile(Lexer &lex, int indent) {
	enterDebug("While");
	println_indent("While", indent);

	if (tokenTryMatchNext(lex, punctLParen)) 
		queue_clean(indent);
	else debug_error();
	if (parserExpr(lex, indent + 1) && tokenTryMatchNext(lex, punctRParen)) {
		queue_clean(indent);
	}
	else debug_error();

	bool ret = true;
	if (lex.get_type() == punctLBrace) {
		ret = parserComplex(lex, indent + 1);
	}
	else {
		ret = parserExpr(lex, indent + 1) && tokenTryMatchNext(lex, punctSemicolon);
		queue_clean(indent);
	}

	if (false == ret) 
		debug_msg("invalid token " + lex.get_token());
	exitDebug();
	return ret;
}

/* pre-matched: 
	"do"
*/
bool Parser::parserDoWhile(Lexer &lex, int indent) {
	enterDebug("DoWhile");
	println_indent("DoWhile", indent);
	queue_clean(indent);

	if (parserComplex(lex, indent + 1) && 
		tokenTryMatchNext(lex, keywordWhile) && 
		tokenTryMatchNext(lex, punctLParen)) 
	{
		queue_clean(indent);
		if (parserExpr(lex, indent + 1) && 
			tokenTryMatchNext(lex, punctRParen) && 
			tokenTryMatchNext(lex, punctSemicolon)) 
		{
			queue_clean(indent);
		}
		else debug_error();
	}
	else debug_error();

	exitDebug();
	return true;
}

/* pre-matched: 
	"for"
*/
bool Parser::parserFor(Lexer &lex, int indent) {
	enterDebug("For");
	println_indent("For", indent);

	if (tokenTryMatchNext(lex, punctLParen)) {
		queue_clean(indent);
		// <var-def>
		if (storageTypeTryMatchNext(lex) || typeTryMatchNext(lex)) 
		{
			typeTryMatchNext(lex);
			if (tokenTryMatchNext(lex, tokenIdent) && 
				parserVarDef(lex, indent + 1)) 
			{}
			else debug_error();
		}
		if (!tokenTryMatchNext(lex, punctSemicolon)) 
			debug_error();
		else queue_clean(indent);

		if (!parserExpr(lex, indent + 1) && 
			tokenTryMatchNext(lex, punctSemicolon)) 
			debug_error();
		queue_clean(indent);
		if (!parserExpr(lex, indent + 1) && 
			tokenTryMatchNext(lex, punctRParen)) 
			debug_error();
		queue_clean(indent);
		if (!parserComplex(lex, indent + 1)) 
			debug_error();
	}
	else debug_error();

	exitDebug();
	return true;
}

static bool unaryTryMatchNext(Lexer &lex) {
	return tokenTryMatchNext(lex, punctPlus) 
		|| tokenTryMatchNext(lex, punctPlusPlus) 
		|| tokenTryMatchNext(lex, punctMinus) 
		|| tokenTryMatchNext(lex, punctMinusMinus) 
		|| tokenTryMatchNext(lex, punctTimes) 
		|| tokenTryMatchNext(lex, punctBitwiseAnd) 
		|| tokenTryMatchNext(lex, punctBitwiseNot) 
		|| tokenTryMatchNext(lex, punctLogicalNot) 
		|| tokenTryMatchNext(lex, keywordSizeof);
}

static bool binaryTryMatchNext(Lexer &lex) {
	return tokenTryMatchNext(lex, punctPlus) 
		|| tokenTryMatchNext(lex, punctPlusAssign) 
		|| tokenTryMatchNext(lex, punctMinus) 
		|| tokenTryMatchNext(lex, punctMinusAssign) 
		|| tokenTryMatchNext(lex, punctTimes) 
		|| tokenTryMatchNext(lex, punctTimesAssign) 
		|| tokenTryMatchNext(lex, punctDivide) 
		|| tokenTryMatchNext(lex, punctDivideAssign) 
		|| tokenTryMatchNext(lex, punctModulo) 
		|| tokenTryMatchNext(lex, punctModuloAssign) 
		|| tokenTryMatchNext(lex, punctAssign) 
		|| tokenTryMatchNext(lex, punctEqual) 
		|| tokenTryMatchNext(lex, punctLess) 
		|| tokenTryMatchNext(lex, punctLessEqual) 
		|| tokenTryMatchNext(lex, punctGreater) 
		|| tokenTryMatchNext(lex, punctGreaterEqual) 
		|| tokenTryMatchNext(lex, punctNotEqual) 
		|| tokenTryMatchNext(lex, punctShr) 
		|| tokenTryMatchNext(lex, punctShrAssign) 
		|| tokenTryMatchNext(lex, punctShl) 
		|| tokenTryMatchNext(lex, punctShlAssign) 
		|| tokenTryMatchNext(lex, punctBitwiseAnd) 
		|| tokenTryMatchNext(lex, punctLogicalAnd) 
		|| tokenTryMatchNext(lex, punctBitwiseAndAssign) 
		|| tokenTryMatchNext(lex, punctBitwiseOr) 
		|| tokenTryMatchNext(lex, punctLogicalOr) 
		|| tokenTryMatchNext(lex, punctBitwiseOrAssign) 
		|| tokenTryMatchNext(lex, punctBitwiseXor) 
		|| tokenTryMatchNext(lex, punctBitwiseXorAssign) 
		|| tokenTryMatchNext(lex, punctComma) 
		|| tokenTryMatchNext(lex, punctPeriod) 
		|| tokenTryMatchNext(lex, punctArrow);
}

static bool literalTryMatchNext(Lexer &lex) {
	return tokenTryMatchNext(lex, tokenInt) 
		|| tokenTryMatchNext(lex, tokenDouble) 
		|| tokenTryMatchNext(lex, tokenChar) 
		|| tokenTryMatchNext(lex, tokenStr);
}

/* pre-matched: 
*/

bool Parser::parserExpr(Lexer &lex, int indent) {
	enterDebug("Expression");
	println_indent("Expression", indent);

	bool flag_paren = tokenTryMatchNext(lex, punctLParen);
	bool ret = true;
	// <unary-expr>
	if (unaryTryMatchNext(lex)) 
		ret = parserUnaryExpr(lex, indent + 1);
	// <literal>
	else if (literalTryMatchNext(lex)) 
		queue_clean(indent);
	// <identfier> or <func-call>
	else if (tokenTryMatchNext(lex, tokenIdent)) {
		if (tokenTryMatchNext(lex, punctLParen)) 
			ret = parserFuncCall(lex, indent + 1);
		else 
			queue_clean(indent);
	}

	// [{ <binary-op> <expression> }] 
	while (ret && binaryTryMatchNext(lex)) {
		queue_clean(indent);
		ret = parserExpr(lex, indent + 1);
	}
	// "?" <expression> ":" <expression>
	if (ret && tokenTryMatchNext(lex, punctQuestion)) {
		queue_clean(indent);
		ret = parserExpr(lex, indent + 1);
		if (!tokenTryMatchNext(lex, punctColon)) 
			debug_error();
		queue_clean(indent);
		ret = parserExpr(lex, indent + 1);
	}

	if (flag_paren) {
		if (tokenTryMatchNext(lex, punctRParen)) 
			queue_clean(indent);
		else 
			debug_error();
	} 

	if (false == ret) 
		debug_error();
	exitDebug();
	return ret;
}

/* pre-matched: 
	<unary-op> 
*/
bool Parser::parserUnaryExpr(Lexer &lex, int indent) {
	enterDebug("UnaryExpr");
	println_indent("Unary Expression", indent);

	queue_clean(indent);
	if (!parserExpr(lex, indent + 1)) 
		debug_error();

	exitDebug();
	return true;
}

/* pre-matched: 
	<identifier> "("
*/
bool Parser::parserFuncCall(Lexer &lex, int indent) {
	enterDebug("FuncCall");
	println_indent("Function Call", indent);

	if (tokenTryMatchNext(lex, tokenIdent) || 
		literalTryMatchNext(lex)) 
	{
		while (tokenTryMatchNext(lex, punctComma)) {
			if (tokenTryMatchNext(lex, tokenIdent) || 
				literalTryMatchNext(lex)) 
				;
			else debug_error();
		}
	}

	if (tokenTryMatchNext(lex, punctRParen)) 
		queue_clean(indent);
	else debug_error();

	exitDebug();
	return true;
}
