//@	{
//@	 "targets":[{"name":"processor_node.test", "type":"application", "autorun":1}]
//@	}

#include "./processor_node.hpp"

#include <cassert>
#include <map>

namespace
{
	class TestProcessor
	{
	public:
		size_t inputCount() const { return 2; }


		std::vector<Texpainter::FilterGraph::ProcResultType> operator()(
		    std::span<Texpainter::FilterGraph::ProcArgumentType const> args) const
		{
			return std::vector<Texpainter::FilterGraph::ProcResultType>{};
		}

		Texpainter::FilterGraph::ProcParamValue get(std::string_view param_name) const
		{
			auto i = m_params.find(param_name);
			if(i != std::end(m_params))
			{ return Texpainter::FilterGraph::ProcParamValue{i->second}; }
			return Texpainter::FilterGraph::ProcParamValue{0.0};
		}

		TestProcessor& set(std::string_view param_name,
		                   Texpainter::FilterGraph::ProcParamValue value)
		{
			m_params[std::string{param_name}] = value.m_value;
			return *this;
		}

	private:
		std::map<std::string, double, std::less<>> m_params;
	};
}

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

	void filtergraphProcessorNodeGetOutput()
	{
		Texpainter::FilterGraph::ProcessorNode node{TestProcessor{}};

		assert(node.inputCount() == 2);
		node.set("Foo", Texpainter::FilterGraph::ProcParamValue{0.5});
		assert(node.get("Foo") == Texpainter::FilterGraph::ProcParamValue{0.5});
	}
};

int main()
{
	Testcases::filtergraphProcessorNodeDummy();

	Testcases::filtergraphProcessorNodeGetOutput();
	return 0;
}