#pragma once

#ifdef _SCRIPT_DLL
#define _SCRIPT_DLL_API __declspec(dllexport) 
#else
#define _SCRIPT_DLL_API
#endif

#include <Windows.h>

extern "C"
{
	class _SCRIPT_DLL_API Script
	{
	public:
		void Destroy(void);

	public:
		int n = 3;
		int x = 2;
	};

	namespace ScriptDll
	{
		_SCRIPT_DLL_API Script * Make(void);
	}
}