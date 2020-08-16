//@	{
//@	 "targets":[{"name":"node_new.test", "type":"application", "autorun":1}]
//@	}

#include "./node_new.hpp"
#include <cassert>

namespace Testcases
{
	void texpainterFilterGraphNodeDefaultState()
	{
		Texpainter::FilterGraph::Node obj;
		assert(!obj.hasProcessor());
		assert(!obj.connected(Texpainter::FilterGraph::InputPort{0}));
		assert(!obj.connected(Texpainter::FilterGraph::InputPort{1}));
		assert(!obj.connected(Texpainter::FilterGraph::InputPort{2}));
		assert(!obj.connected(Texpainter::FilterGraph::InputPort{3}));
		assert(!obj.dirty());
	}
}

#if 0
		explicit Node(std::unique_ptr<AbstractImageProcessor>&& proc);
		Node(): m_dirty{0}, m_proc{nullptr} {}
		auto disconnectedCopy() const { return Node{m_proc->clone()}; }
		Node& replaceWith(std::unique_ptr<AbstractImageProcessor>&& proc);
		AbstractImageProcessor const& processor() const;
		bool hasProcessor() const;
		bool dirty() const;
		result_type const& operator()(Size2d size) const;
		auto inputPorts() const;
		auto outputPorts() const;
		Node& connect(InputPort input, std::reference_wrapper<Node const> other, OutputPort output);
		Node& disconnect(InputPort input);
		std::span<SourceNode const> inputs() const;
		auto paramNames() const;
		auto paramValues() const;
		Node& set(ParamName param_name, ParamValue val);
		ParamValue get(ParamName param_name) const;
		auto name() const;
		~Node();
#endif


int main()
{
	Testcases::texpainterFilterGraphNodeDefaultState();
	return 0;
}