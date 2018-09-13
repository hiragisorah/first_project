#include <iostream>

#include "test_node.hpp"
#include "constant.hpp"

int main(void)
{
	Node::TestNode a;
	Node::TestNode b;
	Node::Constant<int> c(6);

	auto & a1 = a.input_port(0);
	auto & a2 = a.input_port(1);
	auto & a3 = a.input_port(2);

	auto & b1 = b.output_port(0);
	auto & b2 = b.output_port(1);
	auto & b3 = b.output_port(2);

	auto & c1 = c.output_port(0);

	a1->In(c1);
	c1->Out(a1);

	a1->Print();

	rewind(stdin);
	getchar();

	return 0;
}