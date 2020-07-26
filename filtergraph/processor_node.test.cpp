//@	{
//@	 "targets":[{"name":"processor_node.test", "type":"application", "autorun":1}]
//@	}

#include "./processor_node.hpp"

#include <cassert>

namespace Testcases
{
	void filtergraphProcessorNodeDummy()
	{
		Texpainter::FilterGraph::ProcessorNode node;

		auto res = node();
		assert(res.size() == 0);

		assert(&node.set("Foo", Texpainter::FilterGraph::ProcParamValue{0.5}) == &node);
		assert(node.get("Foo") == Texpainter::FilterGraph::ProcParamValue{0.0});
		assert(node.inputCount() == 0);
	}
};

int main()
{
	Testcases::filtergraphProcessorNodeDummy();
	return 0;
}