#include "pch.h"
#include "Error.h"
#include "Parser.h"
#include "ScriptCore.h"
#include <sstream>

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

String ReadWord(const char* ptr, int& len)
{
	String word;
	len = 0;
	while (isalnum(*ptr)) {
		word += *ptr;
		ptr++;
		len++;
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

int BeginParse(Context& c, const char* data, uint off, uint& end, Node** nodePtr)
{
	const char* ptr = data;
	int wordLen = 0;
	c.currentNode = nodePtr;
	Node** prevScope = c.currentNode;

	while (*ptr != '\0' && !isError()) {
		c.considerValue = ReadWord(data, wordLen);
		ptr += wordLen;
		ptr = ReadUntilNonWhite(ptr);
		switch (*ptr)
		{
		case '(':
		{
			ptr++;
			c.conType = ECT::Function;
			int param_count = BaseFunction::GetParamCount(c.considerValue);
			*c.currentNode = FuncNodes[param_count](c.considerValue);
			prevScope = c.currentNode;
			c.currentNode = (*c.currentNode)->GetChild(0);
			String params = CopyUntilWithinScope(ptr, ')', '(', ')');
			auto par = split(params, ',');
			if (!par.empty()) {
				if (par.size() > param_count) warn(("Too many parameters in function call: " + c.considerValue).c_str(), &c);
				for (int i = 0; i < par.size() && i < param_count; i++) {
					uint off = 0;
					BeginParse(c, par[i].c_str(), 0, off, c.currentNode);
					c.currentNode = (*prevScope)->GetChild(i);
				}
			}
			c.considerValue = "";
			ptr = ReadUntilWithinScope(ptr, ')', '(', ')') + 1;
		}
		break;

		case '"':
		{
			ptr++;
			*c.currentNode = new ValueNode(EVT::String, CopyUntil(ptr, '"'));
			ptr = ReadUntil(ptr, '"') + 1;
			c.currentNode = (*c.currentNode)->GetChild();
		}
		break;

		default:
		{
			// Operand, Variable or keyword
		}
			break;
		}
	}

	return false;
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
				String fun = ReadWord(ptr, len);
				ptr += len;
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




bool parseLine(const char* data, uint off, uint& end, Node** nodePtr)
{
	uint count = 0;
	const char* ptr = data + off;
	Node** prevScope = nullptr;
	Node** currentNode = nodePtr;

	String considerValue;
	EConsiderType type = ECT::Null;
	EVariableType valType = EVT::Null;

	bool argument = false;
	int param_count = 0;

	while (*ptr != '\0' && !isError())
	{
		count++;
		if (*ptr == '\n') {
			count++;
			end = count + off;
			return true;
		}


		switch (TypeOfChar(ptr))
		{
		case ECharType::Digit:
		{
			switch (type)
			{
			case EConsiderType::Null:
			{
				type = ECT::Variable;
				valType = EVT::Float;
				considerValue = *ptr;
			} break;
			case EConsiderType::Variable:
			{
				considerValue += *ptr;
			} break;
			case EConsiderType::Function:
				considerValue += *ptr;
				break;
			case EConsiderType::Operator:
				// TODO End operator here!

				type = ECT::Variable;
				valType = EVT::Float;
				considerValue = *ptr;

				break;
			default:
				error("Unexpected error, found digit");
				break;
			}
		}
		break;
		case ECharType::Char:
		{
			switch (type)
			{
			case EConsiderType::Null:
				considerValue += *ptr;
				break;
			case EConsiderType::Variable:
				switch (valType)
				{
				case EVariableType::Null:
					break;
				case EVariableType::Float:
					error("Invalid number");
					break;
				case EVariableType::String:
					considerValue += *ptr;
					break;
				case EVariableType::Object:
					break;
				case EVariableType::Array:
					break;
				default:
					break;
				}
				break;
			case EConsiderType::Function:
				break;
			case EConsiderType::Operator:
				break;
			default:
				break;
			}
		}
		break;
		case ECharType::Operand:
		{
			switch (type)
			{
			case EConsiderType::Null:
				switch (*ptr)
				{
				case '(':
					*currentNode = FuncNodes[BaseFunction::GetParamCount(considerValue)](considerValue);
					prevScope = currentNode;
					currentNode = (*currentNode)->GetChild(0);
					considerValue = "";
					argument = true;
					break;
				case ')':
					argument = false;
					break;
				case ',':
					currentNode = (*prevScope)->GetChild(++param_count);
					break;
				case '"':
					type = ECT::Variable;
					valType = EVT::String;
					break;
				default:
					break;
				}
				break;
			case EConsiderType::Variable:
				switch (*ptr)
				{
				case '"':
					if (valType == EVT::String) {
						*currentNode = new ValueNode(valType, considerValue);
						valType = EVT::Null;
						type = ECT::Null;
					}
					else error("Invalid \" here");
					break;
				default:
					break;
				}
				break;
			case EConsiderType::Function:
				break;
			case EConsiderType::Operator:
				break;
			default:
				break;
			}
		}
		break;
		case ECharType::Space:
		{
			switch (type)
			{
			case EConsiderType::Null:
			{
				// TODO: variable name!

				considerValue = "";
			}
			break;
			case EConsiderType::Variable:
			{
				switch (valType)
				{
				case EVariableType::Null:
					break;
				case EVariableType::Float:
					*currentNode = new ValueNode(valType, considerValue);
					currentNode = (*currentNode)->GetChild();
					considerValue = "";
					type = ECT::Null;
					valType = EVT::Null;
					break;
				case EVariableType::String:
					considerValue += ' ';
					break;
				case EVariableType::Object:
					break;
				case EVariableType::Array:
					break;
				default:
					break;
				}
			} break;
			case EConsiderType::Function:
				// Thinking function arguments?
				break;
			case EConsiderType::Operator:
			{
				// TODO: End operator here!

				considerValue = "";
				type = ECT::Null;
			} break;
			default:
				error("Unexpected whitespace found");
				break;
			}
		}
		break;
		case ECharType::Other:
		{
			error("Weird character found");
		}
		break;
		default:
			break;
		}

		ptr++;
	}

	if (isError()) return false;
	switch (type)
	{
	case EConsiderType::Null:
	{

	}
	break;
	case EConsiderType::Variable:
	{
		*currentNode = new ValueNode(valType, considerValue);
	}
	break;
	case EConsiderType::Function:
	{

	}
	break;
	case EConsiderType::Operator:
	{

	}
	break;
	default:
		break;
	}

	end = count + off;
	return false;
}

void parseBlock()
{

}
