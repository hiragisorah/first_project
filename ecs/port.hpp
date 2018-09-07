#pragma once

#include <typeindex>

namespace Seed
{
	class Port
	{
	public:
		enum class ConnectionType
		{
			Input,
			Output
		};

	public:
		Port(const std::type_index & var_type, const ConnectionType & connection_type)
			: var_type_(var_type)
			, connection_type_(connection_type)
		{}

	private:
		const std::type_index & var_type_;
		const ConnectionType & connection_type_;
	};

	class iInputPort : public Port
	{
	public:
		iInputPort(const std::type_index & var_type)
			: Port(var_type, ConnectionType::Input)
		{}
	};

	class iOutputPort : public Port
	{
	public:
		iOutputPort(const std::type_index & var_type)
			: Port(var_type, ConnectionType::Output)
		{}
	};

	class InputPort : public iInputPort
	{
	public:
		InputPort(void)
			: iInputPort(typeid(int))
		{}
	};

	class OutputPort : public iOutputPort
	{
	public:
		OutputPort(void)
			: iOutputPort(typeid(int))
		{}
	};
}