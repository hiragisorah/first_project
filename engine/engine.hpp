#pragma once

#include <memory>
#include <string>
#include "module.hpp"

namespace Seed
{
	class Engine final
	{
	public:
		Engine(void);
		~Engine(void);

	public:
		const bool Initialize(void);
		const bool Run(void);
		const bool Finalize(void);

	public:
		template<class _Module> _Module * const GetModule(const std::string & module_name)
		{
			return static_cast<_Module*>(this->GetModule(module_name));
		}

	private:
		Seed::Module * const GetModule(const std::string & module_name);

	private:
		class Impl;

		std::unique_ptr<Impl> impl_;
	};
}