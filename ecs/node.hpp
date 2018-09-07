#pragma once

#include <vector>
#include <typeindex>

#include <memory>
#include <unordered_map>



namespace Seed
{
	class Node
	{
	private:
		std::unordered_map<std::string, int> _;

		std::unordered_map<std::type_index, std::unique_ptr<Node>*> components_;
	};
}