#include "Error.h"
#include "Parser.h"
#include "Scope.h"
#include <sstream>

Node* ParseArea(Context& c, const char* const begin, const char* const end);

static const char* returnNode = "e";
static const char* skipNode = "s";

std::vector<String> split(const String& s, char delim) {
	std::vector<String> elems;
	std::istringstream iss(s);
	String item;
	auto result = std::back_inserter(elems);
	while (std::getline(iss, item, delim)) {
		*result++ = item;
	}
	return elems;
}

void ExitContext(Context& c, Context& l)
{
	c.topLevel = l.topLevel;
	c.scope = l.scope;
	c.loopNode = l.loopNode;
	c.loopScope = l.loopScope;
	c.scopeNode = l.scopeNode;
	c.row = l.row;
}

bool isNumber(const String& line, bool* isFloat = nullptr)
{
	if (isFloat) *isFloat = false;
	if (line.size() == 0) return false;
	bool hasDot = false;
	const char* ptr = line.c_str();
	while (*ptr != '\0') {
		if (!(isdigit(*ptr) || (hasDot == false && *ptr == '.'))) return false;
		if (*ptr == '.')
		{
			hasDot = true;
		}
		ptr++;
	}
	if (isFloat) *isFloat = hasDot;
	return true;
}

String ReadWord(Context& c)
{
	bool hasDot = false;
	String word;
	while (isalnum(*c.ptr) || *c.ptr == '_' || (isdigit(word.size() ? *word.begin() : '0') && *c.ptr == '.')) {
		if (*c.ptr == '.')
		{
			if (hasDot) error("Unexpected character: '.'", &c);
			if (!word.size()) word = "0";
			hasDot = true;
		}
		word += *c.ptr;
		c.ptr++;
	}
	return word;
}

String ReadNonAlpha(Context& c)
{
	String word;
	while (ispunct(*c.ptr)) {
		word += *c.ptr;
		c.ptr++;
	}
	return word;
}

inline const char* ReadUntil(const char* ptr, char delim) {
	while (*ptr != delim && *ptr != '\0') ptr++;
	return ptr;
}

const char* ReadUntilWithLimit(const char* ptr, char delim, const char* end, bool* success = nullptr) {
	while (*ptr != delim) {
		if (ptr == end) {
			if (success) *success = false;
			return nullptr;
		}
		ptr++;
	}
	if (success) *success = true;
	return ptr;
}

String CopyUntil(const char* ptr, char delim) {
	String copy;
	while (*ptr != delim) {
		copy += *ptr;
		ptr++;
	}
	return copy;
}

String CopyUntilWithLimit(const char*& ptr, char delim, const char* end, bool* success = nullptr) {
	String copy;
	while (*ptr != delim) {
		if (ptr == end) {
			if (success) *success = false;
			return String();
		}
		copy += *ptr;
		ptr++;
	}
	if (success) *success = true;
	return copy;
}

const char* ReadUntilNonWhite(const char* ptr) {
	while (isspace(*ptr)) ptr++;
	return ptr;
}

const char* ReadUntilNonWhiteWithLimit(const char* ptr, const char* end) {
	while (isspace(*ptr) && ptr != end) ptr++;
	return ptr;
}

const char* ReadUntilWithinScope(const char* ptr, char delim, char scopeBegin = '{', char scopeEnd = '}') {
	int scope = 0;
	while (*ptr && (*ptr != delim || scope != 0)) {
		if (*ptr == scopeBegin) scope++;
		if (*ptr == scopeEnd) scope--;
		ptr++;
	}
	return ptr;
}

const char* ReadUntilWithinScopeWithLimit(const char* ptr, char delim, const char* end, char scopeBegin = '{', char scopeEnd = '}', bool* success = nullptr) {
	int scope = 0;
	if (success) *success = true;
	while (*ptr != delim || scope != 0) {
		if (ptr == end) {
			if (success) *success = false;
			break;
		}
		if (*ptr == scopeBegin) scope++;
		if (*ptr == scopeEnd) scope--;
		ptr++;
	}
	return ptr;
}

String CopyUntilWithinScope(const char*& ptr, char delim, char scopeBegin = '{', char scopeEnd = '}') {
	int scope = 0;
	String copy;
	while (*ptr != delim || scope != 0) {
		if (*ptr == scopeBegin) scope++;
		if (*ptr == scopeEnd) scope--;
		copy += *ptr;
		ptr++;
	}
	return copy;
}

String CopyUntilWithinScopeWithLimit(const char*& ptr, char delim, const char* end, char scopeBegin = '{', char scopeEnd = '}', bool* success = nullptr) {
	int scope = 0;
	String copy;
	if (success) *success = true;
	while ((*ptr != delim || scope != 0) && ptr != end) {
		if (*ptr == scopeBegin) scope++;
		if (*ptr == scopeEnd) scope--;
		copy += *ptr;
		ptr++;
	}
	if (ptr == end && success) *success = false;
	return copy;
}

bool isEqualWord(const String& s, const char* c) {
	for (const char& ch : s) {
		if (*c != ch) return false;
		c++;
	}
	if (isalnum(*c)) return false;
	return true;
}

bool isEqual(const String& s, const char* c) {
	for (const char& ch : s) {
		if (*c != ch) return false;
		c++;
	}
	return true;
}

enum class OpType
{
	Word,
	Two,
	Single
};

#define OPERATION(NAME) {#NAME, OpType::Two, [](Context& c) { \
Node* lhs = ParseArea(c, c.begin, c.ptr); \
Node* rhs = ParseArea(c, c.ptr + sizeof(#NAME) - 1, c.end);\
Node* next = nullptr;\
if (auto lhsv(dynamic_cast<ValueNode*>(lhs)); lhsv) {\
	if (auto rhsv(dynamic_cast<ValueNode*>(rhs)); rhsv) {\
		next = new ValueNode(lhsv->value NAME rhsv->value);\
		delete lhs;\
		delete rhs;\
	}\
	else {\
		next = FuncNodes[2]("op", #NAME, nullptr);\
		next->SetValue(0, lhsv->value);\
		next->SetChild(1, rhs);\
		delete lhs;\
	}\
}\
else {\
	if (auto rhsv(dynamic_cast<ValueNode*>(rhs)); rhsv) {\
		next = FuncNodes[2]("op", #NAME, nullptr);\
		next->SetChild(0, lhs);\
		next->SetValue(1, rhsv->value);\
		delete rhs;\
	}\
	else {\
		next = FuncNodes[2]("op", #NAME, nullptr);\
		next->SetChild(0, lhs);\
		next->SetChild(1, rhs);\
	}\
}\
return next;\
}}\

#define COPERATION(NAME) {#NAME, OpType::Two, [](Context& c) {\
Node* lhs = ParseArea(c, c.begin, c.ptr);\
Node* rhs = ParseArea(c, c.ptr + 2, c.end);\
Node* next = nullptr;\
if (auto lhsv(dynamic_cast<VariableNode*>(lhs)); lhsv && rhs) {\
	next = FuncNodes[2]("op", #NAME, nullptr);\
	lhs->ref = true;\
	next->SetChild(0, lhs);\
	if (auto rhsv(dynamic_cast<ValueNode*>(rhs)); rhsv) {\
		next->SetValue(1, rhsv->value);\
		delete rhs;\
	}\
	else {\
		next->SetChild(1, rhs);\
	}\
	return next;\
}\
else {\
	delete lhs;\
	delete rhs;\
	error("Cannot assign to values", &c);\
	return (Node*)nullptr;\
}\
return next;\
}}\

static std::list<std::tuple<String, OpType, Node* (*)(Context&)>> Operators = {
{"for", OpType::Word, [](Context& c) {
	c.ptr += 3;
	c.ptr = ReadUntilNonWhite(c.ptr);
	if (*c.ptr == '(') {
		c.ptr++;
		bool succ = false;
		String params = CopyUntilWithinScopeWithLimit(c.ptr, ')', c.end, '(', ')', &succ);
		if (!succ) {
			error("Invalid 'for' statement", &c);
			return (Node*)nullptr;
		}
		auto s = split(params, ';');
		if (s.size() != 3) {
			error("Invalid for loop declaration", &c);
			return (Node*)nullptr;
		}
		ForNode* next = new ForNode();
		c.loopScope = c.scope;
		c.scope = new Scope(c.scope, c.topLevel);
		c.scope->parent = c.loopScope;
		c.loopScope->childs.push_back(c.scope);
		c.loopNode = next;
		next->begin = ParseArea(c, s[0].c_str(), s[0].c_str() + s[0].size());
		next->test = ParseArea(c, s[1].c_str(), s[1].c_str() + s[1].size());
		next->end = ParseArea(c, s[2].c_str(), s[2].c_str() + s[2].size());

		Node* rest = ParseArea(c, c.ptr + 1, c.end);
		if (rest) {
			*c.currentNode = next;
			c.currentNode = &(*c.currentNode)->next;
			return rest;
		}
		return (Node*)next;
	}
	error("No statement found", &c);
	return (Node*)nullptr;
}},
{"if", OpType::Word, [](Context& c) {
	c.ptr += 2;
	c.ptr = ReadUntilNonWhite(c.ptr);
	if (*c.ptr == '(') {
		c.ptr++;
		bool succ = false;
		const char* end = ReadUntilWithinScopeWithLimit(c.ptr, ')', c.end, '(', ')', &succ);
		if (!succ) {
			error("Invalid if statement", &c);
			return (Node*)nullptr;
		}
		IfNode* next = new IfNode();
		next->child = ParseArea(c, c.ptr, end);
		Node* rest = ParseArea(c, end + 1, c.end);
		if (rest) {
			*c.currentNode = next;
			c.currentNode = &(*c.currentNode)->next;
			return rest;
		}
		return (Node*)next;
	}
	error("No statement found", &c);
	return (Node*)nullptr;
}},
{"return", OpType::Word, [](Context& c) {
	c.ptr += 7;
	Node* next = new ControlNode();
	next->child = ParseArea(c, c.ptr, c.end);
	return next;
}},
OPERATION(==),
{"!", OpType::Single, [](Context& c) {
	Node* rhs = ParseArea(c, c.ptr + 1, c.end);
	Node* next = nullptr;
	if (auto rhsv(dynamic_cast<ValueNode*>(rhs)); rhsv) {
		next = FuncNodes[1]("op", "!", nullptr);
		next->SetValue(0, rhsv->value);
		delete rhs;
	}
	else {
		next = FuncNodes[1]("op", "!", nullptr);
		next->SetChild(0, rhs);
	}
	return next;
}},
OPERATION(!=),
OPERATION(<=),
OPERATION(>=),
OPERATION(<),
OPERATION(>),
COPERATION(*=),
COPERATION(/=),
COPERATION(+=),
COPERATION(-=),
{"=", OpType::Two, [](Context& c) {
	Node* lhs = ParseArea(c, c.begin, c.ptr);
	auto var(dynamic_cast<VariableNode*>(lhs));
	if (var == nullptr) {
		error("Invalid operand in assignment", &c);
		return (Node*)nullptr;
	}
	if (var->value->type > 0 && var->value->init)
	{
		error("Trying to assing to constant", &c);
		return (Node*)nullptr;
	}
	if (var->value->type == 2) {
		Node* rhs = ParseArea(c, c.ptr + 1, c.end);
		auto rhsv = dynamic_cast<ValueNode*>(rhs);
		if (rhsv) {
			var->value->init = true;
			*var->value->value = rhsv->value;
			delete lhs;
			delete rhs;
			return (Node*)skipNode;
		}
		error("Trying to assing non-constant expression to static variable", &c);
		return (Node*)nullptr;
	}
	var->value->init = true;
	*lhs->GetChild(0) = ParseArea(c, c.ptr + 1, c.end);
	return lhs;
}},
OPERATION(+),
OPERATION(-),
OPERATION(*),
OPERATION(/),
{"-", OpType::Single, [](Context& c) {
	Node* rhs = ParseArea(c, c.ptr + 1, c.end);
	Node* next = nullptr;
	if (auto rhsv(dynamic_cast<ValueNode*>(rhs)); rhsv) {
		rhsv->value = -rhsv->value;
		next = rhs;
	}
	else {
		next = FuncNodes[2]("op", "-", nullptr);
		next->SetChild(1, rhs);
	}
	return next;
}},
{"var", OpType::Word, [](Context& c) {
	c.ptr += 4;
	String name = ReadWord(c);
	if (!name.size()) error("Invalid Variable name: " + name, &c);
	Node* next;
	c.ptr = ReadUntilNonWhite(c.ptr);
	bool isArray = false;
	Value* val = nullptr;
	if (*c.ptr == '[' && *(c.ptr + 1) == ']') {
		val = new Value(EVT::Array, "");
		isArray = true;
	}
	else
		val = new Value();
	if (c.scope) {
		auto it = c.scope->variables.emplace(name, Variable(val, 0, false));
		next = new VariableNode(&it.first->second, isArray);
	}
	else {
		auto it = c.topLevel->vars.emplace(name, Variable(val, 0, false));
		next = new VariableNode(&it.first->second, isArray);
	}
	return next;
}},
{"const", OpType::Word, [](Context& c) {
	c.ptr += 6;
	String name = ReadWord(c);
	if (!name.size()) error("Invalid Variable name: " + name, &c);
	auto it = c.scope->variables.emplace(name, Variable(new Value(), 1, false));
	Node* next = new VariableNode(&it.first->second);
	return next;
}},
{"static", OpType::Word, [](Context& c) {
	c.ptr += 7;
	String name = ReadWord(c);
	if (!name.size()) error("Invalid Variable name: " + name, &c);
	auto it = c.scope->variables.emplace(name, Variable(new Value(), 2, false));
	Node* next = new VariableNode(&it.first->second);
	return next;
}},
{"++", OpType::Single, [](Context& c) {
	Node* lhs = ParseArea(c, c.begin, c.ptr);
	if (auto lhsv(dynamic_cast<VariableNode*>(lhs)); lhsv) {
		Node* next = FuncNodes[2]("op", "+", nullptr);
		VariableNode* nextVar = new VariableNode(lhsv->value);
		next->SetChild(0, nextVar);
		next->SetValue(1, 1LL);
		lhs->SetChild(0, next);
	}
	else {
		error("Cannot increment values", &c);
		delete lhs;
		return (Node*)nullptr;
	}
	return lhs;
}},
{"--", OpType::Single, [](Context& c) {
	Node* lhs = ParseArea(c, c.begin, c.ptr);
	if (auto lhsv(dynamic_cast<VariableNode*>(lhs)); lhsv) {
		Node* next = FuncNodes[2]("op", "-", nullptr);
		VariableNode* nextVar = new VariableNode(lhsv->value);
		next->SetChild(0, nextVar);
		next->SetValue(1, 1LL);
		lhs->SetChild(0, next);
	}
	else {
		error("Cannot decrement values", &c);
		delete lhs;
		return (Node*)nullptr;
	}
	return lhs;
} },
};

Node* ParseArea(Context& c, const char* const begin, const char* const end)
{
	Context l = c;
	Node* result = nullptr;
	l.ptr = begin;
	l.begin = begin;
	l.end = end;
	for (auto const& [key, alnum, func] : Operators) {
		int scope = 0;
		l.ptr = end;
		switch (alnum)
		{
		case OpType::Word:
			while (--l.ptr >= begin) {
				if (*l.ptr == '(') scope++;
				if (*l.ptr == ')') scope--;
				if (*l.ptr == '{') scope++;
				if (*l.ptr == '}') scope--;
				if (scope == 0) {
					if (!isalnum(*l.ptr)) {
						if (*(l.ptr + 1) == *key.c_str() && isEqualWord(key, l.ptr + 1)) {
							l.ptr++;
							result = func(l);
							if (result) break;
							l.ptr--;
						}
					}
					else if (l.ptr == begin) {
						if (*l.ptr == *key.c_str() && isEqualWord(key, l.ptr)) {
							result = func(l);
							if (result) break;
						}
					}
				}
			}
			break;

		case OpType::Two:
			while (--l.ptr >= begin) {
				if (*l.ptr == '(') scope++;
				if (*l.ptr == ')') scope--;
				if (*l.ptr == '{') scope++;
				if (*l.ptr == '}') scope--;
				if (scope == 0 && *l.ptr == *key.c_str() && isEqual(key, l.ptr)) {
					if (l.ptr == begin || *(l.ptr - 1) == '(' || *(l.ptr - 1) == ')' || *(l.ptr - 1) == '{' || *(l.ptr - 1) == '}' || std::isalnum(*(l.ptr - 1)) || std::isspace(*(l.ptr - 1))) {
						const char* p = l.ptr;
						if (p != begin) while (isspace(*(--p)));
						if (isalnum(*p) || *p == ')' || *p == '"') {
							result = func(l);
							if (result) break;
						}
					}
				}
			}
			break;

		case OpType::Single:
			while (--l.ptr >= begin) {
				if (*l.ptr == '(') scope++;
				if (*l.ptr == ')') scope--;
				if (*l.ptr == '{') scope++;
				if (*l.ptr == '}') scope--;
				if (scope == 0 && *l.ptr == *key.c_str() && isEqual(key, l.ptr)) {
					if (l.ptr == begin || *(l.ptr - 1) == '(' || *(l.ptr - 1) == ')' || *(l.ptr - 1) == '{' || *(l.ptr - 1) == '}' || std::isspace(*(l.ptr - 1))) {
						if (isalnum(*(l.ptr + 1))) {
							result = func(l);
							if (result) break;
						}
					}
				}
			}
			break;
		}
		if (result) break;
	}

	l.ptr = begin;
	bool found = false;
	while (!isError() && l.ptr != end && (result == nullptr && !found)) {
		l.ptr = ReadUntilNonWhiteWithLimit(l.ptr, end);
		l.considerValue = ReadWord(l);
		l.ptr = ReadUntilNonWhiteWithLimit(l.ptr, end);
		if (l.ptr == l.end && l.considerValue.size() == 0) break;
		if (*l.ptr == '.') {
			l.ptr++;
			l.considerScope = l.considerValue;
			l.considerValue = ReadWord(l);
			l.ptr = ReadUntilNonWhite(l.ptr);
			if (l.ptr == l.end && l.considerValue.size() == 0) break;
		}
		switch (*l.ptr)
		{
		case '(':
		{
			l.ptr++;
			if (l.considerValue.size() != 0) {
				int param_count = 0;
				if (l.considerScope.size() != 0) {
					Variable* val = l.scope->FindVar(l.considerScope);
					if (val) {
						param_count = GetParamCount(l, "variable", l.considerValue);
						result = FuncNodes[param_count]("variable", l.considerValue, val);
					}
					else if (auto it = c.function->params.find(l.considerScope); it != c.function->params.end()) {
						param_count = GetParamCount(l, "variable", l.considerValue);
						result = FuncNodes[param_count]("variable", l.considerValue, &it->second);
					}
					else if (auto it = nativeFuncs().find(l.considerScope); it != nativeFuncs().end()) {
						param_count = GetParamCount(l, l.considerScope, l.considerValue);
						result = FuncNodes[param_count](l.considerScope, l.considerValue, nullptr);
					}
					else {
						error("Cannot find scope " + l.considerScope, &c);
						break;
					}
					
				}
				else {
					if (l.topLevel->functions.find(l.considerValue) != l.topLevel->functions.end()) {
						param_count = (int)l.topLevel->functions[l.considerValue].params.size();
						result = new ScriptFuncNode(l.topLevel, l.considerValue, param_count);
					}
					else {
						param_count = GetParamCount(l, "global", l.considerValue);
						result = FuncNodes[param_count]("global", l.considerValue, nullptr);
					}
				}
				const char* aend = ReadUntilWithinScopeWithLimit(l.ptr, ')', end, '(', ')');
				std::vector<String> params;
				while (l.ptr != aend && *l.ptr != '\0') {
					l.ptr = ReadUntilNonWhite(l.ptr);
					params.emplace_back(CopyUntilWithinScopeWithLimit(l.ptr, ',', aend, '(', ')'));
					l.ptr = ReadUntilNonWhite(l.ptr);
					if (*l.ptr == ',') l.ptr++;
				}
				if (!params.empty()) {
					if (params.size() > param_count) warn(("Too many parameters in function call: " + l.considerValue).c_str(), &l);
					for (int i = 0; i < params.size() && i < param_count; i++) {
						uint off = 0;
						l.ptr = params[i].c_str();
						result->SetChild(i, ParseArea(l, params[i].c_str(), params[i].c_str() + params[i].size()));
					}
				}
				l.ptr = aend + 1;
				l.considerValue = "";
				break;
			}
			else
			{
				bool success = false;
				l.end = ReadUntilWithinScopeWithLimit(l.ptr, ')', end, '(', ')', &success);
				if (!success) {
					error("Expected ')'", &l);
					result = nullptr;
					break;
				}
				result = ParseArea(l, l.ptr, l.end);
				break;
			}
		}
		break;

		case '"':
		{
			l.ptr++;
			bool succ = false;
			result = new ValueNode(EVT::String, CopyUntilWithLimit(l.ptr, '"', end, &succ));
			l.ptr++;
			if (!succ) error("End of String not found", &c);
			break;
		}
		break;

		case '[':
		{
			l.ptr++;
			l.ptr = ReadUntilNonWhite(l.ptr);
			bool succ = false;
			const char* aend = ReadUntilWithinScopeWithLimit(l.ptr, ']', end, '(', ')', &succ);
			if (succ) {
				Variable* val = l.scope->FindVar(l.considerValue);
				if (val && val->value->type() == EVT::Array) {
					Node* index = ParseArea(l, l.ptr, aend);
					if (val->type == 2) {
						if (ValueNode* n = dynamic_cast<ValueNode*>(index); n) {
							result = new ValueNode(n->value);
						}
						else {
							error("Cannot use non-static access with static variables", &l);
							delete index;
							break;
						}
					}
					else
						result = new VariableNode(val, true, index);
				}
				else
				{
					if (auto it = c.function->params.find(l.considerValue); it != c.function->params.end()) {
						Node* index = ParseArea(l, l.ptr, aend);
						result = new VariableNode(&it->second, true, index);
						break;
					}
					error("Unknown array: " + l.considerValue, &c);
					break;
				}
			}
			else { 
				error("No matching token found: ']'", &l);
				break;
			}
		} break;

		default:
		{
			bool isfloat = false;
			if (isNumber(l.considerValue, &isfloat)) {
				result = new ValueNode(isfloat ? EVT::Float : EVT::Int, l.considerValue);
				break;
			}
			Variable* val = nullptr;
			if (l.scope) val = l.scope->FindVar(l.considerValue);
			else if (l.topLevel->vars.find(l.considerValue) != l.topLevel->vars.end()) { 
				val = &l.topLevel->vars.find(l.considerValue)->second; 
			}
			if (val) {
				if (val->type == 2)
					result = new ValueNode(*val->value);
				else {
					result = new VariableNode(val);
					if (*l.ptr == '&')
						result->ref = true;
				}
				break;
			}
			else if (l.ptr != end && ispunct(*l.ptr)) {
				error((String("Unexpected character found: ") + *l.ptr + ' ').c_str(), &l);
				l.ptr++;
				break;
			}
			else {
				if (l.considerValue == "true") {
					result = new ValueNode(true);
					break;
				}
				else if (l.considerValue == "false") {
					result = new ValueNode(false);
					break;
				}
				else
				{
					if (c.function->params.find(l.considerValue) != c.function->params.end()) {
						result = new VariableNode(&c.function->params[l.considerValue]);
						break;
					}
					error("Unknown variable: " + l.considerValue, &c);
					break;
				}
			}
		}
		break;
		}
	}
	ExitContext(c, l);
	return result;
}

void ReadLine(Context& c)
{
	Context l = c;
	Node** prevScope = nullptr;
	l.begin = c.ptr;
	l.end = l.begin;
	int scope = 0;
	while (*l.end != '\0' && *l.end != '\n' && (*l.end != ';' || scope != 0) && l.end != c.end) {
		if (*l.end == '#') {
			while (*l.end != '\n' && *l.end != '\0') l.end++;
			l.begin = l.end;
		}
		if (*l.end == '(') scope++;
		if (*l.end == ')') scope--;
		if (*l.end == '{') {
			if (l.end != l.begin) {
				break;
			}
			l.scope->childs.push_back(new Scope(l.scope, l.topLevel));
			ScopeNode* sn = new ScopeNode();
			sn->parent = l.scopeNode;
			l.scopeNode = sn;
			*c.currentNode = l.scopeNode;
			c.currentNode = &(*c.currentNode)->child;
			l.scope = l.scope->childs.back();
			sn->scope = l.scope;
			l.begin = ++l.end;
		}
		else if (*l.end == '}') {
			if (l.end != l.begin) {
				break;
			}
			if (l.scope->parent) {
				l.scope = l.scope->parent;
				c.currentNode = &l.scopeNode->next;
				l.scopeNode = l.scopeNode->parent;
				if (l.loopNode) {
					l.scope = l.loopScope;
					l.loopNode = nullptr;
					l.loopScope = nullptr;
				}
			}
			else
			{
				error("Unexpected character found: }", &c);
				break;
			}
			l.begin++;
		}
		l.end++;
	}
	c.ptr = l.end;
	Node* n = ParseArea(l, l.begin, l.end);
	ExitContext(c, l);
	if (n != nullptr) {
		if (n != (Node*)skipNode) {
			while (*c.currentNode) c.currentNode = &(*c.currentNode)->next;
			*c.currentNode = n;
			if (n->next != (Node*)returnNode) c.currentNode = &(*c.currentNode)->next;
			else {
				(*c.currentNode)->next = nullptr;
				c.currentNode = nullptr;
			}
		}
	}
	if (c.ptr != l.end) {
		c.ptr = l.end + 1;
		ReadLine(c);
	}
	ExitContext(c, l);
	c.ptr = l.end;
	if (*l.end == '\n' || *l.end == ';') c.ptr += 1;
	c.row++;
}

int BeginParse(Context& c, const char* data, uint off, uint& end, Node** nodePtr)
{
	Context l = c;
	l.ptr = data;
	int wordLen = 0;
	l.currentNode = nodePtr;

	while (*l.ptr != '\0' && !isError() && l.ptr != c.end && l.currentNode) {
		ReadLine(l);
	}
	c.ptr = l.ptr;
	ExitContext(c, l);
	return true;
}

void FindFunctions(const char* data, Script* script)
{
	const char* function_end = data;
	Context c;
	c.topLevel = script;
	const char* ptr = data;
	int depth = 0;
	int functionCount = 0;
	while (*ptr != '\0') {
		if (*ptr == '\n' || *ptr == ';') c.row++;
		if (*ptr == '{') depth++;
		if (*ptr == '}') depth--;
		if (*ptr == 'd' && depth == 0) {
			if (*(ptr + 1) == 'e' && *(ptr + 2) == 'f' && *(ptr + 3) == ' ') {
				ptr += 4;
				int len = 0;
				c.ptr = ptr;
				String fun = ReadWord(c);
				if (*c.ptr == '(') {
					c.ptr++;
					ScopeNode* scopeN = new ScopeNode();
					script->functions[fun].first = scopeN;
					Node** fir = &(scopeN->child);
					c.scope = new Scope(nullptr, script);
					scopeN->scope = c.scope;
					script->functions[fun].scope = c.scope;
					c.function = &script->functions[fun];
					ptr = ReadUntilWithinScope(ptr, ')');
					while (c.ptr != ptr && *c.ptr != '\0') {
						c.ptr = ReadUntilNonWhite(c.ptr);
						c.function->params[ReadWord(c)] = std::move(Variable(new Value, 0, false));
						bool success = false;
						c.ptr = ReadUntilWithLimit(c.ptr, ',', ptr, &success);
						if (success) c.ptr++;
						else break;
					}
					ptr = ReadUntilWithinScope(ptr, '{') + 1;
					uint off = 0;
					c.end = ReadUntilWithinScope(ptr, '}');
					BeginParse(c, ++ptr, 0, off, fir);
					ptr = c.end + 1;
					function_end = ptr;
				}
				else {
					error("Invalid function definition", &c);
				}
			}
		}
		if (*ptr != '\0')
			ptr++;
	}
	if (script->functions.find("execute") == script->functions.end()) {
		ScopeNode* scopeN = new ScopeNode();
		script->functions["execute"].first = scopeN;
		Node** fir = &(scopeN->child);
		c.scope = new Scope(nullptr, script);
		scopeN->scope = c.scope;
		script->functions["execute"].scope = c.scope;
		c.function = &script->functions["execute"];
		c.begin = function_end;
		c.ptr = function_end;
		c.end = ReadUntil(function_end, '\0');
		uint off = 0;
		BeginParse(c, function_end, 0, off, fir);
	}
}

void FindVariables(const char* data, Script* script)
{
	if (script->setup) delete script->setup;

	script->setup = new ScriptFunction();
	const char* ptr = data;
	Context c;
	c.topLevel = script;
	c.currentNode = &(script->setup->first);
	while (*ptr != '\0') {

		ptr = ReadUntilWithinScope(ptr, 'v');

		if (isEqualWord("var", ptr)) {
			const char* end = ptr;
			while (*end != '\0' && *end != '\n' && *end != ';') end++;
			*c.currentNode = ParseArea(c, ptr, end);
			c.currentNode = &(*c.currentNode)->next;
		}
		ptr++;
	}
}
