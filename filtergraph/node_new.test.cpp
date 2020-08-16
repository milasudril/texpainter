//@	{
//@	 "targets":[{"name":"node_new.test", "type":"application", "autorun":1}]
//@	}

#include "./node_new.hpp"
#include <cassert>

namespace
{
	class ImageProcessorStub final: public Texpainter::FilterGraph::AbstractImageProcessor
	{
		std::array<Texpainter::Memblock, MaxNumOutputs> operator()(
		    Texpainter::FilterGraph::NodeArgument const& arg) const override
		{
			return std::array<Texpainter::Memblock, MaxNumOutputs>{};
		}

		std::span<Texpainter::FilterGraph::PortInfo const> inputPorts() const override
		{
			return std::span<Texpainter::FilterGraph::PortInfo const>{};
		}

		std::span<Texpainter::FilterGraph::PortInfo const> outputPorts() const override
		{
			return std::span<Texpainter::FilterGraph::PortInfo const>{};
		}

		std::span<Texpainter::FilterGraph::ParamName const> paramNames() const override
		{
			return std::span<Texpainter::FilterGraph::ParamName const>{};
		}

		std::span<Texpainter::FilterGraph::ParamValue const> paramValues() const override
		{
			return std::span<Texpainter::FilterGraph::ParamValue const>{};
		}

		Texpainter::FilterGraph::ParamValue get(
		    Texpainter::FilterGraph::ParamName param_name) const override
		{
			return Texpainter::FilterGraph::ParamValue{0};
		}

		Texpainter::FilterGraph::AbstractImageProcessor& set(
		    Texpainter::FilterGraph::ParamName param_name,
		    Texpainter::FilterGraph::ParamValue value) override
		{
			return *this;
		}

		std::unique_ptr<Texpainter::FilterGraph::AbstractImageProcessor> clone() const override
		{
			return std::make_unique<ImageProcessorStub>(*this);
		}

		char const* name() const override { return "ImageProcessorStub"; }
	};
}

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

	void texpainterFilterGraphNodeDisconnectedCopy()
	{
		Texpainter::FilterGraph::Node obj{std::make_unique<ImageProcessorStub>()};
		assert(obj.hasProcessor());
		auto other = obj.disconnectedCopy();
		assert(other.hasProcessor());
		assert(&other.processor() != &obj.processor());
	}
}

#if 0
		explicit Node(std::unique_ptr<AbstractImageProcessor>&& proc);
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
	Testcases::texpainterFilterGraphNodeDisconnectedCopy();
	return 0;
}