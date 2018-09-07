#pragma once

#ifdef _MAIN_WINDOW_DLL
#define _MAIN_WINDOW_DLL_API __declspec(dllexport) 
#else
#define _MAIN_WINDOW_DLL_API
#endif

#include <seed/module>
#include <string>
#include <memory>

#pragma warning (disable: 4251)

namespace Seed
{
	class MainWindow : public Seed::Module
	{
	public:
		MainWindow(void);
		~MainWindow(void);

	private:
		const bool Initialize(void) override;
		const bool Run(void) override;
		const bool Finalize(void) override;
		void Destroy(void) override;

	private:
		class Impl;

		std::unique_ptr<Impl> impl_;
	};

	namespace MainWindowDLL
	{
		_MAIN_WINDOW_DLL_API Seed::MainWindow * Make(void);
	}
}