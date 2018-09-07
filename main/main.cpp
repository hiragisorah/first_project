#include <seed/dll_loader>
#include <seed/graphics>
#include <seed/input>
#include <seed/window>
#include <seed/engine>

#include <iostream>
#include <filesystem>

int __stdcall WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Seed::Engine engine;

	engine.Initialize();

	while (engine.Run());

	engine.Finalize();

	return 0;
}