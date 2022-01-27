#include "pch.h"
#include "Export.h"

#include "defines.h"
#include <map>
#include <list>
#include <vector>

#include "Parser.h"
#include "Error.h"
#include "ScriptCore.h"


static std::map<uint, Script*> Scripts;
uint activeScript = 0;

unsigned __int32 CompileScript(const char* data)
{
	ClearError();
	uint off = 0;
	uint idx = Scripts.size() ? Scripts.rbegin()->first + 1 : 1;
	Scripts[idx] = new Script();
	activeScript = idx;
	Parser::FindVariables(data, Scripts[idx]);
	Parser::FindFunctions(data, Scripts[idx]);

	if (isError()) {
		CleanScript(idx);
		return 0;
	}

	Value v;
	Scripts[idx]->setup->invoke(v);

	return idx;
}

void EvaluateScript(unsigned __int32 s)
{
	if (Scripts.find(s) == Scripts.end()) return;
	Scripts[s]->evaluate();
}

void AddFuncs(std::any fun)
{
	//globalFuncs["funner"] = make_wrap<Value, Value>(fun);
}

void CleanScript(unsigned __int32 script)
{
	if (Scripts.find(script) != Scripts.end()) delete Scripts[script];
	Scripts.erase(script);
}

void GetError(const char* error, size_t size)
{

}
