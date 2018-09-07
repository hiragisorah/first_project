#pragma once

#include <Windows.h>
#include <memory>
#include <string>

namespace Seed
{
	template<class _Class>
	struct ClassDeleter
	{
		void operator()(_Class * pointer) const { pointer->Destroy(); }
	};

	template<class _Class>
	class DLLLoader
	{
		typedef _Class*(*MakeFunc)(void);

		struct LibraryDeleter
		{
			typedef HMODULE pointer;
			inline void operator()(HMODULE module) const { FreeLibrary(module); }
		};

	private:
		std::unique_ptr<HMODULE, LibraryDeleter> dll_;
		MakeFunc make_;

	public:
		DLLLoader(const std::string & dll_path)
			: dll_(LoadLibraryA(dll_path.c_str()))
		{
			this->make_ = (MakeFunc)GetProcAddress(this->dll_.get(), "Make");
		}

	public:
		std::unique_ptr<_Class, ClassDeleter<_Class>> SmartMake(void)
		{
			return std::unique_ptr<_Class, ClassDeleter<_Class>>((*this->make_)());
		}
		_Class * const Make(void)
		{
			return (*this->make_)();
		}
	};
}