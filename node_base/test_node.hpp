#pragma once

#include "node.hpp"

namespace Node
{
	class TestNode : public Seed::iNode
	{
	public:
		TestNode(void)
		{
			this->AddInputPort<int>("test");
			this->AddInputPort<float>("test2");
			this->AddInputPort<std::string>("test3");

			auto a1 = this->AddOutputPort<std::string>("best1");
			auto a2 = this->AddOutputPort<int>("best2");
			auto a3 = this->AddOutputPort<float>("best3");

			a1->Set("xx");
			a2->Set(3);
			a3->Set(0.5f);
		}

	private:


	public:
		void Update(void) override
		{
			
		}
	};
}