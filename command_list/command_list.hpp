#pragma once

#include <functional>

template<class _Class>
class CommandList
{
	using Command = std::function<void(_Class&)>;
	using SortCommand = std::function<bool(_Class&, _Class&)>;

private:
	std::vector<Command> command_s_;
	std::vector<_Class> args_s_;
	
public:
	const CommandList & operator += (const Command & command)
	{
		this->command_s_.emplace_back(command);
		return *this;
	}

	const CommandList & operator +=(_Class && args)
	{
		this->args_s_.emplace_back(std::move(args));
		return *this;
	}
	const CommandList & operator +=(_Class & args)
	{
		this->args_s_.emplace_back(args);
		return *this;
	}

	void operator () (void)
	{
		for (auto & command : this->command_s_)
		{
			for (auto & args : this->args_s_)
			{
				command(args);
			}
		}

		this->command_s_.clear();

		this->args_s_.clear();
	}

	void operator () (const bool & clear_commands, const bool & clear_args)
	{
		for (auto & command : this->command_s_)
		{
			for (auto & args : this->args_s_)
			{
				command(args);
			}
		}
		
		if(clear_commands)
			this->command_s_.clear();
		
		if(clear_args)
			this->args_s_.clear();
	}

	void operator >>= (const SortCommand & sort_command)
	{
		std::sort(this->args_s_.begin(), this->args_s_.end(), sort_command);
	}
};