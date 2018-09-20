#pragma once

#include <vector>
#include <typeindex>
#include <functional>
#include <seed/math>

namespace Seed
{
	using PortType = std::type_index;
	using PortTypeCollection = std::vector<PortType>;

	class Node
	{
	public:
		Node(void) {};
		virtual ~Node(void) {};

	public:
		virtual void Input(const unsigned int & port_num, const PortType & port_type, void * value) = 0;
		virtual void * Output(const unsigned int & port_num, const PortType & port_type, void * value) = 0;

	public:
		template<class _PortType> void add_input_port(void)
		{
			this->input_port_collection_.emplace_back(typeid(_PortType));
		}
		template<class _PortType> void add_output_port(void)
		{
			this->output_port_collection_.emplace_back(typeid(_PortType));
		}

	private:
		std::string name_;
		Math::float3 position_;

		PortTypeCollection input_port_collection_;
		PortTypeCollection output_port_collection_;

	public:
		const std::string & name(void)
		{
			return this->name_;
		}
		const Math::float3 & position(void)
		{
			return this->position_;
		}
		void position(const Math::float3 & position)
		{
			this->position_ = position;
		}
		const PortTypeCollection & input_port_collection(void)
		{
			return this->input_port_collection_;
		}
		const PortTypeCollection & output_port_collection(void)
		{
			return this->output_port_collection_;
		}
	};
}