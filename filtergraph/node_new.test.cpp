//@	{
//@	 "targets":[{"name":"node_new.test", "type":"application", "autorun":1}]
//@	}

#include "./node_new.hpp"
#include <cassert>

namespace
{
	class ImageProcessorStub final: public Texpainter::FilterGraph::AbstractImageProcessor
	{
	public:
		static constexpr std::array<Texpainter::FilterGraph::PortInfo const, 2> s_input_ports{
		    {{Texpainter::FilterGraph::PixelType::RGBA, "Input 1"},
		     {Texpainter::FilterGraph::PixelType::GrayscaleReal, "Input 2"}}};

		static constexpr std::array<Texpainter::FilterGraph::PortInfo const, 3> s_output_ports{
		    {{Texpainter::FilterGraph::PixelType::RGBA, "Output 1"},
		     {Texpainter::FilterGraph::PixelType::GrayscaleComplex, "Output 2"},
		     {Texpainter::FilterGraph::PixelType::GrayscaleReal, "Output 3"}}};

		bool no_inputs{false};

	private:
		std::array<Texpainter::Memblock, MaxNumOutputs> operator()(
		    Texpainter::FilterGraph::NodeArgument const& arg) const override
		{
			return std::array<Texpainter::Memblock, MaxNumOutputs>{
			    Texpainter::Memblock{1}, Texpainter::Memblock{1}, Texpainter::Memblock{1}};
		}

		std::span<Texpainter::FilterGraph::PortInfo const> inputPorts() const override
		{
			return no_inputs ? std::span<Texpainter::FilterGraph::PortInfo const>{} : s_input_ports;
		}

		std::span<Texpainter::FilterGraph::PortInfo const> outputPorts() const override
		{
			return s_output_ports;
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
		assert(!obj.isConnected(Texpainter::FilterGraph::InputPort{0}));
		assert(!obj.isConnected(Texpainter::FilterGraph::InputPort{1}));
		assert(!obj.isConnected(Texpainter::FilterGraph::InputPort{2}));
		assert(!obj.isConnected(Texpainter::FilterGraph::InputPort{3}));
		assert(!isConnected(obj));
	}

	void texpainterFilterGraphNodeDisconnectedCopy()
	{
		Texpainter::FilterGraph::Node obj{std::make_unique<ImageProcessorStub>()};
		assert(obj.hasProcessor());
		auto other = obj.disconnectedCopy();
		assert(other.hasProcessor());
		assert(&other.processor() != &obj.processor());
	}

	void texpainterFilterGraphNodeReplaceWith()
	{
		Texpainter::FilterGraph::Node obj{std::make_unique<ImageProcessorStub>()};
		assert(obj.hasProcessor());
		auto old = &obj.processor();
		obj.replaceWith(std::make_unique<ImageProcessorStub>());
		assert(obj.hasProcessor());
		assert(old != &obj.processor());
	}

	void texpainterFilterGraphNodeCall()
	{
		Texpainter::FilterGraph::Node obj{std::make_unique<ImageProcessorStub>()};
		assert(obj.hasProcessor());

		ImageProcessorStub input_stub;
		input_stub.no_inputs = true;
		Texpainter::FilterGraph::Node input{std::make_unique<ImageProcessorStub>(input_stub)};

		obj.connect(
		       Texpainter::FilterGraph::InputPort{0}, input, Texpainter::FilterGraph::OutputPort{0})
		    .connect(Texpainter::FilterGraph::InputPort{1},
		             input,
		             Texpainter::FilterGraph::OutputPort{2});
		obj(Texpainter::Size2d{1, 1});
	}
}

#if 0
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
	Testcases::texpainterFilterGraphNodeReplaceWith();
	Testcases::texpainterFilterGraphNodeCall();
	return 0;
}