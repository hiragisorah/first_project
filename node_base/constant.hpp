#pragma once

#include "node.hpp"

namespace Node
{
	template<class _VarType>
	class Constant : public Seed::iNode
	{
	public:
		template<class ... Args>
		Constant(const Args &... args)
		{
			this->AddOutputPort<_VarType>("variable")->Set(args ...);
		}
	
	public:
		void Update(void) override
		{

		}
	};
}