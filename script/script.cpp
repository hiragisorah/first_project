#include "script.hpp"

_SCRIPT_DLL_API Script * ScriptDll::Make(void)
{
	return new Script;
}

void Script::Destroy(void)
{
	delete this;
}
