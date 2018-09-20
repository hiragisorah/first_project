#include <Windows.h>
#include <seed/engine>
#include <seed/command_list>

int __stdcall WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	CommandList<std::unique_ptr<Seed::Engine>> command_list;

	command_list += [](auto & engine) { engine->Initialize(); };
	command_list += [](auto & engine) { while (engine->Run()); };
	command_list += [](auto & engine) { engine->Finalize(); };
	
	command_list += std::make_unique<Seed::Engine>();

	command_list();

	return 0;
}