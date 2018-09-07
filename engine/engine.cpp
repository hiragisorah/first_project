#include "engine.hpp"

#include <seed/dll_loader>

#include <filesystem>
#include <unordered_map>

class Seed::Engine::Impl
{
	friend Seed::Engine;

public:
	Impl(void);
	~Impl(void);

private:
	std::unordered_map<std::string, Seed::Module*> module_mapped_;
	std::vector<Seed::DLLLoader<Seed::Module>> dll_s_;

};

Seed::Engine::Engine(void)
	: impl_(std::make_unique<Seed::Engine::Impl>())
{

}

Seed::Engine::~Engine(void)
{

}

const bool Seed::Engine::Initialize(void)
{
	namespace fs = std::filesystem;

	std::vector<std::string> dll_name_s;

	fs::path p("../Debug/data/module/"); // start
	std::for_each(fs::directory_iterator(p), fs::directory_iterator(),
		[&dll_name_s](const fs::path& p) {
		if (fs::is_regular_file(p)) // is file
			if (p.extension() == ".dll") // is dll
			{
				dll_name_s.emplace_back(p.filename().string());
			}
	});

	for (auto & dll_name : dll_name_s)
	{
		this->impl_->dll_s_.emplace_back("data/module/" + dll_name);
		this->impl_->module_mapped_[dll_name] = this->impl_->dll_s_.back().Make();
	}

	for (auto & module : this->impl_->module_mapped_)
		if (!module.second->Initialize())
			return false;

	return true;
}

const bool Seed::Engine::Run(void)
{
	for (auto & module : this->impl_->module_mapped_)
		if (!module.second->Run())
			return false;

	return true;
}

const bool Seed::Engine::Finalize(void)
{
	for (auto & module : this->impl_->module_mapped_)
		if (!module.second->Finalize())
			return false;

	return true;
}

Seed::Module * const Seed::Engine::GetModule(const std::string & module_name)
{
	return this->impl_->module_mapped_[module_name];
}

Seed::Engine::Impl::Impl(void)
{

}

Seed::Engine::Impl::~Impl(void)
{
	for (auto & module : this->module_mapped_)
		if (module.second)
			module.second->Destroy();
}
