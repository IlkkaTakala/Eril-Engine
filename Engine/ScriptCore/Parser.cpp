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

static std::map<String, Node*(*)(Context&)> Operators = {
	{"=", [](Context& c) {
			//*c.currentNode = FuncNodes[2](s);
			/*Node** lhs = (*c.currentNode)->GetChild(0);
			Node** rhs = (*c.currentNode)->GetChild(1);
			*lhs = MakeValueNode(c);
			*rhs = MakeValueNode(c);
			c.currentNode = &(*c.currentNode)->next;*/
			return FuncNodes[2]("=");
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

String CopyUntil(const char* ptr, char delim) {
	String copy;
	while (*ptr != delim) {
		copy += *ptr;
		ptr++;
	}
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

void HandleVariable(Context& c)
{

}

bool isEqual(const String& s, const char* c) {
	for (const char& ch : s) {
		if (*c != ch) return false;
	}
	return true;
}

Node* ParseArea(Context& c, const char* const begin, const char* const end)
{
	int scope = 0;
	Context l = c;
	Node* result = nullptr;
	l.ptr = begin;
	l.begin = begin;
	l.end = end;
	for (auto const& [key, func] : Operators) {
		l.ptr = end;
		while (--l.ptr >= begin) {
			if (*l.ptr == '(') scope++;
			if (*l.ptr == ')') scope--;
			if (*l.ptr == '{') scope++;
			if (*l.ptr == '}') scope--;
			if (scope == 0 && *l.ptr == *key.c_str() && isEqual(key, l.ptr)) {
				result = func(l);
				return result;
			}
		}
	}
	l.ptr = begin;

	while (l.ptr != end) {
		l.considerValue = ReadWord(l);
		l.ptr = ReadUntilNonWhite(l.ptr);
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
			return result;
		}
		break;

		case '"':
		{
			l.ptr++;
			result = new ValueNode(EVT::String, CopyUntil(l.ptr, '"'));
			l.ptr = ReadUntil(l.ptr, '"') + 1;

			return result;
		}
		break;

		default:
		{
			// Operator, Variable or keyword
			if (isNumber(l.considerValue)) {
				result = new ValueNode(EVT::Float, l.considerValue);
				return result;
			}
			else if (ispunct(*l.ptr)) {
				error((String("Unexpected character found: ") + *l.ptr + ' ').c_str(), &l);
				l.ptr++;
			}
			//if (l.considerValue.empty()) l.ptr++;
		}
		break;
		}
	}
	//error("Something went wrong here", &l);
	return nullptr;
}

void ReadLine(Context& c)
{
	Context l = c;
	Node** prevScope = nullptr;
	l.begin = c.ptr;
	l.end = l.ptr;
	while (*l.end != '\0' && *l.end != '\n' && *l.end != ';') l.end++;

	*c.currentNode = ParseArea(l, l.begin, l.end);
	
	c.ptr = l.end;
	c.row++;
}

int BeginParse(Context& c, const char* data, uint off, uint& end, Node** nodePtr)
{
	Context l;
	l.ptr = data;
	l.row = c.row;
	int wordLen = 0;
	l.currentNode = nodePtr;
	Node** prevScope = l.currentNode;

	while (*l.ptr != '\0' && !isError()) {
		ReadLine(l);
	}
	return true;
}

void Parser::FindFunctions(const char* data, Script* script)
{
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
				String fun = ReadWord(c);
				if (*ptr == '(') {
					Node** fir = &(script->functions[fun].first);
					ptr = ReadUntilWithinScope(ptr, '{');
					uint off = 0;
					BeginParse(c, ++ptr, 0, off, fir);
					ptr += off;
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
		uint off = 0;
		BeginParse(c, data, 0, off, fir);
	}
}