#include <stack>
#include <iostream>
#include <stdexcept>

#include "../parser"

#pragma GCC dependency ../parser

static std::stack<std::string> s;

void enterDebug(std::string node_name) {
	s.push(node_name);
}

void exitDebug(void) {
	s.pop();
}

void debug_msg(std::string msg) {
	std::cerr << s.top() << ": " 
		<< msg << std::endl;
}

void debug_msg(std::string msg, int line) {
	std::cerr << s.top() << "at line " << line 
		<< ": " << msg << std::endl;
}

static void tokenMatchNext(Lexer &lex) {
	std::cout << "matched:" << lex.get_line() 
		<< ":" << lex.get_lineChar() << ": '"
		<< lex.get_token() << "'" << std::endl;
	lex.next();
}

static bool tokenTryMatchNext(Lexer &lex, token_t type) {
	if (lex.get_type() == type) {
		tokenMatchNext(lex);
		return true;
	}
	else 
		return false;
}

static ast *parserCode(Lexer &lex);
static ast *parserLine(Lexer &lex);
static ast *parserIf(Lexer &lex);
static ast *parserWhile(Lexer &lex);
static ast *parserDoWhile(Lexer &lex);
static ast *parserFor(Lexer &lex);

static ast *parserComma(Lexer &lex);
static ast *parserAssign(Lexer &lex);
static ast *parserTernary(Lexer &lex);
static ast *parserBool(Lexer &lex);
static ast *parserEquality(Lexer &lex);
static ast *parserShift(Lexer &lex);
static ast *parserExpr(Lexer &lex);
static ast *parserTerm(Lexer &lex);
static ast *parserUnary(Lexer &lex);
static ast *parserPostUnary(Lexer &lex);
static ast *parserObject(Lexer &lex);
static ast *parserFactor(Lexer &lex);
static ast *parserElementInit(Lexer &lex);
static ast *parserDesinatedInit(Lexer &lex);

static ast *parserType(Lexer &lex);
static ast *parserDecl(Lexer &lex, bool mod);

static ast *parserStorage(Lexer &lex);
static ast *parserFnImpl(Lexer &lex);
static ast *parserField(Lexer &lex);
static ast *parserEnumField(Lexer &lex);
static ast *parserParam(Lexer &lex);
static ast *parserDeclBasic(Lexer &lex);
static ast *parserStructOrUnion(Lexer &lex);
static ast *parserEnum(Lexer &lex);
static ast *parserDeclExpr(Lexer &lex, bool inDecl);
static ast *parserDeclUnary(Lexer &lex);
static ast *parserDeclObject(Lexer &lex);
static ast *parserParamList(Lexer &lex);
static ast *parserDeclAtom(Lexer &lex);
static ast *parserName(Lexer &lex);

ast *parser(Lexer &lex) {
	debugEnter("Module");

	ast *Module = new ast(lex.get_pos(), astModule);
	ast *tmp;

	while (lex.get_type() != tokenEOF) {
		if (tokenTryMatchNext(lex, punctSemicolon)) 
			;
		else if (lex.get_type() == tokenPreprocessor
				|| lex.get_type() == tokenComment) 
			tokenMatchNext(lex);
		else {
			tmp = parserDecl(lex);
			if (tmp != nullptr) 
				Module->add_child(tmp);
		}
	}
	exitDebug();

	return Module;
}

static ast *parserCode(Lexer &lex) {
	enterDebug("Code");

	ast *ret = new ast(lex.get_pos(), astCode);
	bool flag = false;

	flag = tokenTryMatchNext(lex, punctLBrace);
	ret->add_child(parserLine(lex));
	// if braces are not in pairs
	if (flag && !tokenTryMatchNext(lex, punctRBrace)) {
		debug_msg("'}' required", lex.get_line());
		delete Node;
		Node = nullptr;
	}
	
	exitDebug();
	return ret;
}

static ast *parserLine(Lexer &lex) {
	enterDebug("Line");
	ast *ret = nullptr;

	if (lex.get_type() == keywordIf) 
		ret = parserIf(lex);
	else if (lex.get_type() == keywordWhile) 
		ret = parserWhile(lex);
	else if (lex.get_type() == keyworDo) 
		ret = parserDoWhile(lex);
	else if (lex.get_type() == keywordFor) 
		ret = parserFor(lex);
	else if (lex.get_type() == punctLBrace) 
		ret = parserCode(lex);
	else if (tokenIsDecl(lex)) 
		ret = parserDecl(lex, false);
	else {
		Lexer::pos_t pos = lex.get_pos();
		if (tokenTryMatchNext(lex, keywordReturn)) {
			ret = new ast(pos, astReturn);
			if (lex.get_type() != punctSemicolon) 
				ret->r = parserComma(lex);
		}
		else if (tokenTryMatchNext(lex, keywordBreak)) 
			ret = new ast(pos, astBreak);
		else if (tokenTryMatchNext(lex, keywordContinue)) 
			ret = new ast(pos, astContinue);
		else if (lex.get_type() == punctSemicolon) 	// empty line
			ret = new ast(pos, astEmpty);
		else 
			ret = new ast(pos, astComma);

		tokenMatchNext(lex, punctSemicolon);
	}

	exitDebug();
	return ret;
}

static ast *parserIf(Lexer &lex) {
	enterDebug("If");

	ast *ret = new ast(lex.get_pos(), astIf);
	if (tokenTryMatchNext(lex, keywordIf) 
	  || tokenTryMatchNext(lex, punctLParen)) {
		ret->add_child(parserComma(lex));
		if (!tokenTryMatchNext(lex, punctRParen)) {
			debug_msg("missing ')'", lex.get_line());
			delete ret;
			ret = nullptr;
		}
		ret->l = parserCode(lex);
		if (tokenTryMatchKeyword(lex, keywordElse)) 	
			ret->r = parserCode(lex);
	}
	else {
		debug_msg("invalid form", lex.get_line());
		delete ret;
		ret = nullptr
	}

	exitDebug();
	return ret;
}

static ast *parserWhile(Lexer &lex) {
	enterDebug("While");

	ast *ret = new ast(lex.get_pos(), astWhile);
	if (tokenTryMatchNext(lex, keywordWhile) 
	  || tokenTryMatchNext(lex, keyword, punctLParen)) {
		ret->add_child(parserComma(lex));
		if (tokenTryMatchNext(lex, punctRParen)) {
			debug_msg("missing ')'", lex.get_line());
			delete ret;
			ret = nullptr;
		}
		ret->add_child(parserCode(lex);
	}
	else {
		debug_msg("invalid form", lex.get_line());
		delete ret;
		ret = nullptr;
	}

	exitDebug();
	return ret;
}

static ast *parserDoWhile(Lexer &lex) {
	enterDebug("DoWhile");
	ast *ret = new ast(lex.get_pos(), astDoWhile);

	if (tokenTryMatchNext(lex, keywordDo)) {
		ret->add_child(parserCode(lex));
		if (tokenTryMatchNext(lex, keywordWhile) 
		  || tokenTryMatchNext(lex, punctLParen)) {
			ret->add_child(parserComma(lex));
			if (!tokenTryMatchNext(lex, punctRParen)) {
				debug_msg("missing ')'", lex.get_line());
				delete ret;
				ret = nullptr;
			}
			else if (!tokenTryMatchNext(lex, punctSemicolon)) {
				debug_msg("missing ';'", lex.get_line());
				delete ret;
				ret = nullptr;
			}
		}
		else {
			debug_msg("invalid form", lex.get_line());
			delete ret;
			ret = nullptr;
		}
	}
	else {
		debug_msg("missing 'do'", lex.get_line());
		delete ret;
		ret = nullptr;
	}

	exitDebug();
	return ret;
}

static ast *parserFor(Lexer &lex) {
	enterDebug("For");
	ast *ret = new ast(lex.get_pos(), astFor);

	if (tokenTryMatchNext(lex, keywordFor) 
	  && tokenTryMatchNext(lex, punctLParen)) {
		if (tokenIsDecl(lex)) 
			ret->add_child(parserDecl(lex, false));
		else {
			if (lex->get_type != punctSemicolon) 
				ret->add_child(parserComma(lex));
			else 
				ret->add_child(new ast(lex.get_pos(), astEmpty));
			if (!tokenTryMatchNext(lex, punctSemicolon)) {
				debug_msg("missing ';'", lex.get_line());
				delete ret;
				ret = nullptr;
			}
		}

		if (lex->get_type != pnctSemicolon) 
			ret->add_child(parserComma(lex));
		else 
			ret->add_child(new ast(lex.get_pos(), astEmpty));
		if (!tokenTryMatchNext(lex, punctSemicolon)) {
			debug_msg("missing ';'", lex.get_line());
			delete ret;
			ret = nullptr;
		}
		if (lex->get_type != pnctSemicolon) 
			ret->add_child(parserComma(lex));
		else 
			ret->add_child(new ast(lex.get_pos(), astEmpty));
		if (!tokenTryMatchNext(lex, punctRParen)) {
			debug_msg("missing ')'", lex.get_line());
			delete ret;
			ret = nullptr;
		}

		ret->add_child(parserCode(lex));
	}
	else {
		debug_msg("invalid form", lex.get_line());
		delete ret;
		ret = nullptr;
	}

	exitDebug();
	return ret;
}

static ast *parserComma(Lexer &lex) {
	enterDebug("Comma");
	ast *ret = new ast(lex.get_pos(), astComma);

	do {
		ret->add_child(parserAssign(lex));
	} while (tokenTryMatchNext(lex, punctComma));

	exitDebug();
	return ret;
}

static ast *parserAssign(Lexer &lex) {
	enterDebug("Assign");
	ast *ret = new ast(lex.get_pos(), astAssign);
	ast *tmp = parserTernary(lex);

	if (tmp == nullptr) {
		debug_msg("error occurs when creating Ternary node");
		throw runtime_error();
	}

	ret->left = tmp;
	ret->op = lex.get_token();
	if (ret->op == punctBitwiseAndAssign 
		|| ret->op == punctBitwiseOrAssign 
		|| ret->op == punctBitwiseXorAssign 
		|| ret->op == punctShrAssign 
		|| ret->op == punctShlAssign 
		|| ret->op == punctAssign 
		|| ret->op == punctPlusAssign 
		|| ret->op == punctMinusAssign 
		|| ret->op == punctTimesAssign 
		|| ret->op == punctDivideAssign ) 
	{
		tokenMatchNext(lex);
		tmp = parserAssign(lex);
		if (tmp == nullptr) {
			debug_msg("error occurs when creating node Assign");
			throw runtime_error();
		}
		ret->right = tmp;
	}
	else ret->op = tokenUndefined;

	exitDebug();
	return ret;
}

static ast *parserTernary(Lexer &lex) {
	enterDebug("Ternary");
	ast *ret = new ast(lex.get_pos(), astTernary);

	ret->add_child(parserBool(lex));

	if (tokenTryMatchNext(lex, punctQuestion)) {
		tokenMatchNext(lex);
		ret->add_child(parserComma(lex));
		if (!tokenTryMatchNext(lex, punctColon)) {
			debug_msg("missing ':'");
			delete ret;
			ret = nullptr;
		}
		else 
			ret->add(parserTernary(lex));
	}

	exitDebug();
	return ret;
}

static ast *parserBool(Lexer &lex) {
	enterDebug("Bool");

	ast *ret = new ast(lex.get_pos(), astBool);
	ast *tmp = new ast(lex.get_pos(), astEquality);

	if (nullptr == tmp) {
		debug_msg("error occurs when creating node Equality");
		throw runtime_error();
	}

	ret->left = tmp;
	ret->op = lex.get_type();
	if (ret->op == punctLogicalAnd 
		|| ret->op == punctLogicalOr 
		|| ret->op == punctLogicalNot 
		|| ret->op == punctBitwiseAnd 
		|| ret->op == punctBitwiseOr 
		|| ret->op == punctBitwiseXor) 
	{
		tokenMatchNext();
		tmp = new ast(lex.get_pos(), astEquality);
		if (nullptr == tmp) {
			debug_msg("error occurs when creating node Equality");
			throw runtime_error();
		}
		ret->right = tmp;
	}
	else 
		ret->op = tokenUndefined;

	exitDebug();
	return ret;
}

static ast *parserEquality(Lexer &lex) {
	enterDebug("Equality");

	ast *ret = new ast(lex.get_pos(), astEquality);
	ast *tmp = parserShift(lex);

	if (nullptr == tmp) {
		debug_msg("error occurs when creating node Shift");
		throw runtime_error();
	}

	ret->left = tmp;
	ret->op = lex.get_type();
	if (ret->op == punctEqual 
		|| ret->op == punctNotEqual 
		|| ret->op == punctGreater 
		|| ret->op == punctGreaterEqual 
		|| ret->op == punctLess 
		|| ret->op == punctLessEqual) 
	{
		tokenMatchNext();
		tmp = parserShift(lex);
		if (nullptr == tmp) {
			debug_msg("error occurs when creating node Shift");
			throw runtime_error();
		}
		else 
			ret->right = tmp;
	}
	else 
		ret->op = tokenUndefined;

	exitDebug();
	return ret;
}

static ast *parserShift(Lexer &lex) {
	enterDebug("Shift");
	ast *ret = new ast(lex.get_pos(), astShift);
	ast *tmp = parserExpr(lex);

	if (nullptr == tmp) {
		debug_msg("error occurs when creating node Expr");
		throw runtime_error();
	}

	ret->left = tmp;
	ret->op = lex.get_type();
	if (ret->op == punctShr || ret->op == punctShl) {
		tokenMatchNext();
		tmp = parserExpr(lex);
		if (nullptr == tmp) {
			debug_msg("error occurs when creating node Expr");
			throw runtime_error();
		}
		ret->right = tmp;
	}
	else ret->op = tokenUndefined;

	exitDebug();
	return ret;
}

static ast *parserExpr(Lexer *lex) {
	enterDebug("Expr");
	ast *ret = new ast(lex.get_pos(), astExpr);
	ast *tmp = parserTerm(lex);

	if (nullptr == tmp) {
		debug_msg("error occurs when creating node Term");
		throw runtime_error();
	}

	ret->left = tmp;
	ret->op = lex.get_type();

	if (ret->op == punctPlus || ret->op == punctMinus) {
		tokenMatchNext(lex);
		tmp = parserTerm(lex);
		if (nullptr == tmp) {
			debug_msg("error occurs when creating node Term");
			throw runtime_error();
		}
		ret->right = tmp;
	}
	else ret->op = tokenUndefined;

	exitDebug();
	return ret;
}

static ast *parserTerm(Lexer &lex) {
	enterDebug("Term");
	ast *ret = new ast(lex.get_pos(), astTerm);
	ast *tmp = parserUnary(lex);

	if (nullptr == tmp) {
		debug_msg("error occurs when creating node Unary");
		throw runtime_error();
	}
	ret->left = tmp;
	ret->op = lex.get_type();
	if (ret->op == punctTimes ||
		ret->op == punctDivide ||
		ret->op == punctModulo) 
	{
		tokenMatchNext(lex);
		tmp = parserUnary(lex);
		if (nullptr == tmp) {
			debug_msg("error occurs when creating node Unary");
			throw runtime_error();
		}
		ret->right = tmp;
	}
	else ret->op = tokenUndefined;

	exitDebug();
	return ret;
}

static ast *parserUnary(Lexer &lex) {
	enterDebug("Unary");
	ast *ret = new ast(lex.get_pos(), astUnary);
	
	ret->op = lex.get_type();
	if (ret->op == punctLogicalNot || 
		ret->op == punctBitwiseNot ||
		ret->op == punctMinus ||
		ret->op == punctTimes ||
		ret->op == punctBitwiseAnd ||
		ret->op == punctPlusPlus ||
		ret->op == punctMinusMinus) 
	{
		tokenMatchNext();
		ast *tmp = parserUnary(lex);
		if (nullptr == tmp) {
			debug_msg("error occurs when creating node Unary");
			throw runtime_error();
		}
		ret->right = tmp;
	}
	else {		// PostUnary
		ret->op = tokenUndefined;
		ast *tmp = parserPostUnary(lex);
		if (nullptr == tmp) {
			debug_msg("error occurs when creating node PostUnary");
			throw runtime_error();
		}
		ret->right = tmp;
	}

	exitDebug();
	return ret;
}

static ast *parserPostUnary(Lexer &lex) {
	enterDebug("PostUnary");
	ast *ast = new ast(lex.get_pos(), astPostUnary);
	ast *tmp = parserObject(lex);

	if (nullptr == tmp) {
		debug_msg("error occurs when creating Object");
		throw runtime_error();
	}

	ast->left = tmp;
	ast->op = lex.get_type();

	if (tokenTryMatchNext(lex, punctPlusPlus) ||
		tokenTryMatchNext(lex, punctMinusMinus) 
		;
	else {
		debug_msg("missing '++' or '--'");
		delete ret;
		ret = nullptr;
	}

	exitDebug();
	return ret;
}

static ast *parserObject(Lexer &lex) {
	enterDebug("Object");
	ast *ret = new ast(lex.get_pos(), astObject);

	while (true) {
		// "[" Comma "]"
		if (tokenTryMatchNext(lex, punctLBracket)) {
			ret->add_child(parserComma(lex));
			if (!tokenTryMatchNext(lex, punctRBracket)) {
				debug_msg("missing ']'");
				delete ret;
				ret = nullptr;
				break;
			}
		}
		// "(" [Assign [{"," Assign}]] ")"
		else if (tokenTryMatchNext(lex, punctLParen)) {
			ret->add_child(parserAssign(lex));
			while (tokenTryMatchNext(lex, punctComma)) {
				ret->add_child(parserAssign(lex));
			}
			if (!tokenTryMatchNext(lex, punctRParen)) {
				debug_msg("missing ')'");
				delete ret;
				ret = nullptr;
				break;
			}
		}
		// "." | "->" <Ident>
		else if (tokenTryMatchNext(lex, punctPeriod) ||
			tokenTryMatchNext(lex, punctArrow)) 
		{
			if (lex.get_type() == tokenIdent) 
				ret->add_child(new ast(lex.get_pos(), tokenIdent));
			else {
				debug_msg("identifier expected");
				throw runtime_error();
			}
		}
		else 
			break;
	}

	exitDebug();
	return ret;
}

static ast *parserFactor(Lexer &lex) {
	enterDebug("Factor");
	ast *ret = new ast(lex.get_pos(), astFactor);

	if (tokenTryMatchNext(lex, punctLParen)) {
		// "(" Type ")" Unary
		if (tokenIsDecl(lex)) {
			ret->add_child(parserType(lex));
			tokenMatchNext(lex, punctRParen);
			ret->add_child(parserUnary(lex));
		}
		// "(" Comma ")"
		else 
			ret->add_child(parserComma(lex));
	}
	// "sizeof" "(" Type | Comma ")"
	else if (tokenTryMatchNext(lex, keywordSizeof)) {
		if (!tokenTryMatchNext(lex, punctLParen)) {
			debug_msg("missing '('");
			throw runtime_error();
		}
		if (tokenIsDecl(lex)) 
			ret->add_child(parserType(lex));
		else 
			ret->left = parserComma(lex);
		if (!tokenTryMatchNext(lex, punctRParen)) {
			debug_msg("missing ')'");
			throw runtime_error();
		}
	}
	else if (lex.get_type() == keywordInt ||
		lex.get_type() == keywordBool ||
		lex.get_type() == tokenStr ||
		lex.get_type() == tokenIdent ||
		lex.get_type() == keywordChar) 
	{
		ret->add_child(new ast(lex.get_pos(), lex.get_type()));
		tokenMatchNext(lex);
	}
	else {
		debug_msg("error occurs");
		throw runtime_error();
	}

	exitDebug();
	return ret;
}

static ast *parserType(Lexer &lex) {
	enterDebug("Type");
	ast *ret = new ast(lex.get_pos(), astType);
	ret->left = new ast(lex.get_pos(), parserDeclBasic(lex));
	ret->right = new ast(lex.get_pos(), parserDeclExpr(lex));

	exitDebug();
	return ret;
}

static ast *parserDecl(Lexer &lex, bool mod) {
	debugEnter("Decl");

	ast *ret = new ast(lex.get_pos(), astDecl);
	ret->left = parserStorage(lex);
	ret->right = parserDeclBasic(lex);

	if (tokenTryMatchNext(lex, punctSemicolon)) 
		;
	else {
		ret->add_child(parserDeclExpr(lex, true));

		if (lex.get_type() == punctLBrace) {
			if (!mod) {
				debug_msg("invalid decl");
				throw runtime_error();
			}
			ret->add_child(parserCode(lex));
		}
		else while (tokenTryMatchNext(lex, punctComma)) {
			ret->add_child(parserDeclExpr(lex, true));
		}
	}

	exitDebug();
	return ret;
}

static ast *parserStorage(Lexer &lex) {
	enterDebug("Storage");
	ast *ret = nullptr;

	if (lex.get_type() == keywordAuto ||
		lex.get_type() == keywordStatic ||
		lex.get_type() == keywordExtern ||
		lex.get_type() == keywordTypedef) 
	{
		ret = new ast(lex.get_pos(), lex.get_type());
		tokenMatchNext(lex);
	}

	exitDebug();
	return ret;
}

static ast *parserField(Lexer &lex) {
	debugEnter("Field");

}
