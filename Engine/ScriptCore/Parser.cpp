#include "pch.h"
#include "Error.h"
#include "Parser.h"
#include "ScriptCore.h"


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

const char* ReadWord(const char* ptr, int& len)
{
	String word;
	len = 0;
	while (isalnum(*ptr)) {
		word += *ptr;
		ptr++;
		len++;
	}
	return word.c_str();
}

const char* ReadUntil(const char* ptr, char delim) {
	while (*ptr != delim) ptr++;
	return ptr;
}

const char* ReadUntilWithinScope(const char* ptr, char delim) {
	int scope = 0;
	while (*ptr != delim || scope != 0) {
		if (*ptr == '{') scope++;
		if (*ptr == '}') scope--;
		if (*ptr == delim && scope == 0) break;
		ptr++;
	}
	return ptr;
}

int Parser::BeginParse(const char* data, uint off, uint& end, Node** nodePtr)
{
	return false;
}

void Parser::ParseFunctions(Script* script)
{
	//Node** ptr = &(Scripts[idx]->functions["execute"].first);
}

void Parser::FindFunctions(const char* data, Script* script)
{
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
				const char* fun = ReadWord(ptr, len);
				ptr += len;
				if (*ptr == '(') {
					Node** fir = &(script->functions[fun].first);
					printf("Found function!\n");
					ptr = ReadUntilWithinScope(ptr, '{');
				}
				else {
					error("Invalid function definition");
				}
			}
		}



		ptr++;
	}
	if (script->functions.find("execute") == script->functions.end()) {
		Node** fir = &(script->functions["execute"].first);

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
