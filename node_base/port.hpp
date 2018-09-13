#pragma once

#include <string>
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
		Port(const std::type_index & var_type, const ConnectionType & connection_type, const std::string & label)
			: var_type_(var_type)
			, connection_type_(connection_type)
			, label_(label)
		{}

	private:
		const std::type_index var_type_;
		const ConnectionType connection_type_;
		const std::string label_;

	public:
		const std::type_index & var_type(void) const
		{
			return this->var_type_;
		}
		const std::string & label(void) const
		{
			return this->label_;
		}

		virtual void Print(void) {}
	};

	class iOutputPort;

	class iInputPort : public Port
	{
	public:
		iInputPort(const std::type_index & var_type, const std::string & label)
			: Port(var_type, ConnectionType::Input, label)
		{}

	public:
		virtual void In(const std::shared_ptr<iOutputPort> & output_port) = 0;
	};

	class iOutputPort : public Port
	{
	public:
		iOutputPort(const std::type_index & var_type, const std::string & label)
			: Port(var_type, ConnectionType::Output, label)
		{}

	public:
		virtual void Out(const std::shared_ptr<iInputPort> & input_port) = 0;
	};

	template<class _VarType>
	class InputPort;

	template<class _VarType>
	class OutputPort : public iOutputPort
	{
	public:
		OutputPort(const std::string & label)
			: iOutputPort(typeid(_VarType), label)
		{}

	private:
		std::weak_ptr<InputPort<_VarType>> out_;

	public:
		void Out(const std::shared_ptr<iInputPort> & input_port) override
		{
			if (input_port->var_type() == this->var_type())
				this->out_ = std::static_pointer_cast<InputPort<_VarType>>(input_port);
		}

	private:
		_VarType var_;

	public:
		const _VarType & Get(void) const
		{
			return this->var_;
		}
		void Set(const _VarType & var)
		{
			this->var_ = var;
		}

	public:
		void Print(void) override
		{
			std::cout << this->var_ << std::endl;
		}
	};

	template<class _VarType>
	class InputPort : public iInputPort
	{
	public:
		template<class ... Args>
		InputPort(const std::string & label)
			: iInputPort(typeid(_VarType), label)
		{}

	private:
		std::weak_ptr<OutputPort<_VarType>> in_;

	public:
		void In(const std::shared_ptr<iOutputPort> & output_port) override
		{
			if (output_port->var_type() == this->var_type())
				this->in_ = std::static_pointer_cast<OutputPort<_VarType>>(output_port);
		}

	public:
		const _VarType & Get(void)
		{
			return this->in_.lock()->Get();
		}

	public:
		void Print(void) override
		{
			std::cout << this->in_.lock()->Get() << std::endl;
		}
	};
}