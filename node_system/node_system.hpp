#pragma once

#include "node.hpp"

#include <seed/graphics>

#include <vector>
#include <memory>

namespace Seed
{
	using NodeCollection = std::vector<Node*>;

	class iNodeSystem
	{
	public:
		iNodeSystem(const std::unique_ptr<iGraphics> & graphics) : graphics_(graphics) {};

		virtual ~iNodeSystem(void)
		{
			for (auto & node : this->node_collection_)
				delete node;
		}

	protected:
		const std::unique_ptr<iGraphics> & graphics_;
		NodeCollection node_collection_;
		
	public:
		template<class _Node, class ... Args> void add_node(const Args &... args)
		{
			this->node_collection_.emplace_back(new Node(args ...));
		}

	public:
		virtual void Initialize(void) = 0;

		void Draw(void)
		{
			this->DrawBackground();
			
			for (auto & node : this->node_collection_)
				this->DrawNode(node);
		}

	private:
		virtual void DrawBackground(void) = 0;
		virtual void DrawNode(Node * const node)
		{
			auto & position = node->position();
			auto & input_port_collection = node->input_port_collection();
			auto & output_port_collection = node->output_port_collection();
			auto input_port_cnt = input_port_collection.size();
			auto output_port_cnt = output_port_collection.size();
		}
	};
}