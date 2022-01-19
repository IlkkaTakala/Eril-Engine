#include "pch.h"
#include "Error.h"
#include "Parser.h"
#include "ScriptCore.h"
#include <sstream>

Node* ParseArea(Context& c, const char* const begin, const char* const end);

template <typename Out>
void split(const String& s, char delim, Out result) {
	std::istringstream iss(s);
	String item;
	while (std::getline(iss, item, delim)) {
		*result++ = item;
	}
}

std::vector<String> split(const String& s, char delim) {
	std::vector<String> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}

void ExitContext(Context& c, Context& l)
{
	c.scope = l.scope;
}

inline ECharType TypeOfChar(const char* c)
{
	if (isdigit(*c)) {
		return ECharType::Digit;
	}
	else if (isalpha(*c)) {
		return ECharType::Char;
	}
	else if (isspace(*c)) {
		return ECharType::Digit;
	}
	else if (ispunct(*c)) {
		return ECharType::Operand;
	}
	else
		return ECharType::Other;
}

bool isNumber(const String& line)
{
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
	return true;
}

Node* MakeValueNode(Context& c)
{
	if (c.considerValue.size() == 0) return new ValueNode(EVT::Null, "");
	if (isNumber(c.considerValue)) return new ValueNode(EVT::Float, c.considerValue);
	return new ValueNode(EVT::String, c.considerValue);
}

String ReadWord(Context& c)
{
	bool hasDot = false;
	String word;
	//len = 0;
	while (isalnum(*c.ptr) || (isdigit(word.size() ? *word.begin() : '0') && *c.ptr == '.')) {
		if (*c.ptr == '.')
		{
			if (hasDot) error("Unexpected character: '.'", &c);
			if (!word.size()) word = "0";
			hasDot = true;
		}
		word += *c.ptr;
		c.ptr++;
		//len++;
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

const char* ReadUntil(const char* ptr, char delim) {
	while (*ptr != delim) ptr++;
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

String CopyUntilWithLimit(const char* ptr, char delim, const char* end, bool* success = nullptr) {
	String copy;
	while (*ptr != delim) {
		if (ptr == end) {
			if (success) *success = false;
			return nullptr;
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

const char* ReadUntilWithinScope(const char* ptr, char delim, char scopeBegin = '{', char scopeEnd = '}') {
	int scope = 0;
	while (*ptr != delim || scope != 0) {
		if (*ptr == scopeBegin) scope++;
		if (*ptr == scopeEnd) scope--;
		if (*ptr == delim && scope == 0) break;
		ptr++;
	}
	return ptr;
}

String CopyUntilWithinScope(const char* ptr, char delim, char scopeBegin = '{', char scopeEnd = '}') {
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

bool isEqual(const String& s, const char* c) {
	for (const char& ch : s) {
		if (*c != ch) return false;
		c++;
	}
	return true;
}

static std::list<std::tuple<String, Node* (*)(Context&)>> Operators = {
{"=", [](Context& c) {
	Node* lhs = ParseArea(c, c.begin, c.ptr);
	if (static_cast<VariableNode*>(lhs) == nullptr) {
		error("Invalid operand in assignment", &c);
		return (Node*)nullptr;
	}
	*lhs->GetChild(0) = ParseArea(c, c.ptr + 1, c.end);
	return lhs;
}},
{"-", [](Context& c) {
	Node* next = FuncNodes[2]("-");
	*next->GetChild(0) = ParseArea(c, c.begin, c.ptr);
	*next->GetChild(1) = ParseArea(c, c.ptr + 1, c.end);

	return next;
}},
{"+", [](Context& c) {
	Node* next = FuncNodes[2]("+");
	*next->GetChild(0) = ParseArea(c, c.begin, c.ptr);
	*next->GetChild(1) = ParseArea(c, c.ptr + 1, c.end);

	return next;
}},
{"/", [](Context& c) {
	return FuncNodes[2]("/");
}},
{"*", [](Context& c) {
	return FuncNodes[2]("*");
}},
};


static std::map<String, Node* (*)(Context&)> Keywords = {
{"var ", [](Context& c) {
	c.ptr += 4;
	String name = ReadWord(c);
	if (!name.size()) error("Invalid Variable name: " + name, &c);
	auto it = c.scope->variables.emplace(name, new Value());
	Node* next = new VariableNode(it.first->second);
	return next;
}}
};

Node* ParseArea(Context& c, const char* const begin, const char* const end)
{
	Context l = c;
	Node* result = nullptr;
	l.ptr = begin;
	l.begin = begin;
	l.end = end;
	for (auto const& [key, func] : Operators) {
		int scope = 0;
		l.ptr = end;
		while (--l.ptr >= begin) {
			if (*l.ptr == '(') scope++;
			if (*l.ptr == ')') scope--;
			if (*l.ptr == '{') scope++;
			if (*l.ptr == '}') scope--;
			if (scope == 0 && *l.ptr == *key.c_str() && isEqual(key, l.ptr)) {
				result = func(l);
				break;
			}
		}
		if (result) break;
	}
	if (!result)
	for (auto const& [key, func] : Keywords) {
		int scope = 0;
		l.ptr = end;
		while (--l.ptr >= begin) {
			if (*l.ptr == '(') scope++;
			if (*l.ptr == ')') scope--;
			if (*l.ptr == '{') scope++;
			if (*l.ptr == '}') scope--;
			if (scope == 0) {
				if (!isalnum(*l.ptr)) {
					if (*(l.ptr + 1) == *key.c_str() && isEqual(key, l.ptr + 1)) {
						l.ptr++;
						result = func(l);
						break;
					}
				}
				else if (l.ptr == begin) {
					if (*l.ptr == *key.c_str() && isEqual(key, l.ptr)) {
						result = func(l);
						break;
					}
				}
			}
		}
		if (result) break;
	}

	l.ptr = begin;
	bool found = false;
	while (!isError() && l.ptr != end && (result == nullptr && !found)) {
		l.ptr = ReadUntilNonWhite(l.ptr);
		l.considerValue = ReadWord(l);
		l.ptr = ReadUntilNonWhite(l.ptr);
		if (l.ptr == l.end && l.considerValue.size() == 0) break;
		switch (*l.ptr)
		{
		case '(':
		{
			l.ptr++;
			l.conType = ECT::Function;
			int param_count = BaseFunction::GetParamCount(l.considerValue);
			result = FuncNodes[param_count](l.considerValue);
			String params = CopyUntilWithinScope(l.ptr, ')', '(', ')');
			auto par = split(params, ',');
			if (!par.empty()) {
				if (par.size() > param_count) warn(("Too many parameters in function call: " + l.considerValue).c_str(), &l);
				for (int i = 0; i < par.size() && i < param_count; i++) {
					uint off = 0;
					l.ptr = par[i].c_str();
					*(result)->GetChild(0) = ParseArea(l, par[i].c_str(), par[i].c_str() + par[i].size());
				}
			}
			l.ptr = end;
			l.considerValue = "";
			break;
		}
		break;

		case '"':
		{
			l.ptr++;
			bool succ = false;
			result = new ValueNode(EVT::String, CopyUntilWithLimit(l.ptr, '"', end, &succ));
			l.ptr = ReadUntilWithLimit(l.ptr, '"', end, &succ) + 1;
			if (!succ) error("End of String not found", &c);
			break;
		}
		break;

		/*case '{':
		{
			l.scope->childs.push_back(new Scope(l.scope));
			l.scope = l.scope->childs.back();
			l.ptr++;
			result = ParseArea(l, l.ptr, l.end);
			break;
		} break;

		case '}':
		{
			if (l.scope->parent) l.scope = l.scope->parent;
			else
			{
				error("Unexpected character found: }", &c);
				break;
			}
			l.ptr++;
			result = ParseArea(l, l.ptr, l.end);
			break;
		} break;*/

		default:
		{
			// Operator, Variable or keyword
			if (isNumber(l.considerValue)) {
				result = new ValueNode(EVT::Float, l.considerValue);
				break;
			}
			else if (l.ptr != end && ispunct(*l.ptr)) {
				error((String("Unexpected character found: ") + *l.ptr + ' ').c_str(), &l);
				l.ptr++;
				break;
			}
			else {

				Value* val = l.scope->FindVar(l.considerValue);
				if (val) {
					result = new VariableNode(val);
					break;
				}
				else
				{
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
	while (*l.end != '\0' && *l.end != '\n' && *l.end != ';' && l.end != c.end) {
		if (*l.end == '{') {
			l.scope->childs.push_back(new Scope(l.scope));
			l.scope = l.scope->childs.back();
			l.begin = ++l.end;
		}
		else if (*l.end == '}') {
			if (l.end != l.begin) {
				break;
			}
			if (l.scope->parent) l.scope = l.scope->parent;
			else
			{
				error("Unexpected character found: }", &c);
				break;
			}
			l.begin++;
		}
		l.end++;
	}

	Node* n = ParseArea(l, l.begin, l.end);
	ExitContext(c, l);
	if (n != nullptr) {
		*c.currentNode = n;
		c.currentNode = &(*c.currentNode)->next;
	}
	c.ptr = l.end;
	if (*l.end == '\n' || *l.end == ';') c.ptr += 1;
	c.row++;
}

int BeginParse(Context& c, const char* data, uint off, uint& end, Node** nodePtr)
{
	Context l = c;
	l.ptr = data;
	//l.row = c.row;
	int wordLen = 0;
	l.currentNode = nodePtr;
	//Node** prevScope = l.currentNode;

	while (*l.ptr != '\0' && !isError() && l.ptr != c.end) {
		ReadLine(l);
	}
	c.ptr = l.ptr;
	return true;
}

void Parser::FindFunctions(const char* data, Script* script)
{
	const char* function_end = data;
	Context c;
	const char* ptr = data;
	int depth = 0;
	int functionCount = 0;
	while (*ptr != '\0') {
		if (*ptr == '{') depth++;
		if (*ptr == '}') depth--;
		if (*ptr == 'd' && depth == 0) {
			if (*(ptr + 1) == 'e' && *(ptr + 2) == 'f' && *(ptr + 3) == ' ') {
				ptr += 4;
				int len = 0;
				c.ptr = ptr;
				String fun = ReadWord(c);
				if (*c.ptr == '(') {
					Node** fir = &(script->functions[fun].first);
					c.scope = new Scope(nullptr);
					script->functions[fun].scope = c.scope;
					ptr = ReadUntilWithinScope(ptr, '{');
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



		ptr++;
	}
	if (script->functions.find("execute") == script->functions.end()) {
		Node** fir = &(script->functions["execute"].first);
		c.scope = new Scope(nullptr);
		script->functions["execute"].scope = c.scope;
		c.begin = function_end;
		c.ptr = function_end;
		c.end = ReadUntil(function_end, '\0');
		uint off = 0;
		BeginParse(c, function_end, 0, off, fir);
	}
}