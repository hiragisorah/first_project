#pragma once

#include <memory>
#include <vector>

#include "port.hpp"

namespace Seed
{
	class iNode
	{
	private:
		std::vector<std::shared_ptr<Seed::iInputPort>> input_port_s_;
		std::vector<std::shared_ptr<Seed::iOutputPort>> output_port_s_;

	public:
		template<class _VarType> std::shared_ptr<Seed::InputPort<_VarType>> AddInputPort(const std::string & label)
		{
			auto shared = std::make_shared<Seed::InputPort<_VarType>>(label);
			this->input_port_s_.emplace_back(shared);
			return shared;
		}
		template<class _VarType> std::shared_ptr<Seed::OutputPort<_VarType>> AddOutputPort(const std::string & label)
		{
			auto shared = std::make_shared<Seed::OutputPort<_VarType>>(label);
			this->output_port_s_.emplace_back(shared);
			return shared;
		}

	public:
		const std::shared_ptr<Seed::iInputPort> & input_port(const unsigned int & num)
		{
			return this->input_port_s_[num];
		}
		const std::shared_ptr<Seed::iOutputPort> & output_port(const unsigned int & num)
		{
			return this->output_port_s_[num];
		}

	public:
		virtual void Update(void) = 0;
	};
}