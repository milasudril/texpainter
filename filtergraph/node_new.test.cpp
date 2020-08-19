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

		static constexpr std::array<Texpainter::FilterGraph::ParamName const, 3> s_param_names{
		    "Param 1", "Param 2", "Param 3"};


		static constexpr std::array<Texpainter::FilterGraph::ParamValue const, 3> s_param_values{
		    Texpainter::FilterGraph::ParamValue{1.0},
		    Texpainter::FilterGraph::ParamValue{2.0},
		    Texpainter::FilterGraph::ParamValue{3.0}};

		bool no_inputs{false};

		Texpainter::FilterGraph::ParamValue* called_with_paramvalue{nullptr};
		Texpainter::FilterGraph::ParamName* called_with_paramname{nullptr};

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
			return s_param_names;
		}

		std::span<Texpainter::FilterGraph::ParamValue const> paramValues() const override
		{
			return s_param_values;
		}

		Texpainter::FilterGraph::ParamValue get(
		    Texpainter::FilterGraph::ParamName param_name) const override
		{
			*called_with_paramname = param_name;
			return Texpainter::FilterGraph::ParamValue{0};
		}

		Texpainter::FilterGraph::AbstractImageProcessor& set(
		    Texpainter::FilterGraph::ParamName param_name,
		    Texpainter::FilterGraph::ParamValue value) override
		{
			*called_with_paramname  = param_name;
			*called_with_paramvalue = value;
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

	void texpainterFilterGraphNodeInputPorts()
	{
		Texpainter::FilterGraph::Node obj{std::make_unique<ImageProcessorStub>()};
		assert(obj.hasProcessor());
		auto ports = obj.inputPorts();
		assert(std::ranges::equal(ports, ImageProcessorStub::s_input_ports));
	}

	void texpainterFilterGraphNodeOutputPorts()
	{
		Texpainter::FilterGraph::Node obj{std::make_unique<ImageProcessorStub>()};
		assert(obj.hasProcessor());
		auto ports = obj.outputPorts();
		assert(std::ranges::equal(ports, ImageProcessorStub::s_output_ports));
	}

	void texpainterFilterGraphNodeDisconnect()
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
		assert(isConnected(obj));

		obj.disconnect(Texpainter::FilterGraph::InputPort{0});
		assert(!obj.isConnected(Texpainter::FilterGraph::InputPort{0}));
		assert(obj.isConnected(Texpainter::FilterGraph::InputPort{1}));
		assert(!isConnected(obj));
	}

	void texpainterFilterGraphNodeInputs()
	{
		Texpainter::FilterGraph::Node obj{std::make_unique<ImageProcessorStub>()};
		assert(obj.hasProcessor());

		assert((std::ranges::none_of(obj.inputs(), [](auto item) { return item.valid(); })));

		ImageProcessorStub input_stub;
		input_stub.no_inputs = true;
		Texpainter::FilterGraph::Node input{std::make_unique<ImageProcessorStub>(input_stub)};

		obj.connect(
		       Texpainter::FilterGraph::InputPort{0}, input, Texpainter::FilterGraph::OutputPort{0})
		    .connect(Texpainter::FilterGraph::InputPort{1},
		             input,
		             Texpainter::FilterGraph::OutputPort{2});

		assert((std::ranges::all_of(obj.inputs(), [](auto item) { return item.valid(); })));
		std::array<Texpainter::FilterGraph::Node const*, 2> nodes{&input, &input};
		assert((std::ranges::equal(obj.inputs(), nodes, [](auto const& item_a, auto const& item_b) {
			return &item_a.processor() == item_b;
		})));

		std::array<Texpainter::FilterGraph::OutputPort, 2> output_ports{
		    Texpainter::FilterGraph::OutputPort{0}, Texpainter::FilterGraph::OutputPort{2}};
		assert((std::ranges::equal(
		    obj.inputs(), output_ports, [](auto const& item_a, auto const& item_b) {
			    return item_a.port() == item_b;
		    })));
	}

	void texpainterFilterGraphNodeParamNames()
	{
		Texpainter::FilterGraph::Node obj{std::make_unique<ImageProcessorStub>()};
		assert(obj.hasProcessor());
		assert(std::ranges::equal(obj.paramNames(), ImageProcessorStub::s_param_names));
	}

	void texpainterFilterGraphNodeParamValues()
	{
		Texpainter::FilterGraph::Node obj{std::make_unique<ImageProcessorStub>()};
		assert(obj.hasProcessor());
		assert(std::ranges::equal(obj.paramNames(), ImageProcessorStub::s_param_names));
	}

	void texpainterFilterGraphNodeSetParam()
	{
		auto proc = std::make_unique<ImageProcessorStub>();
		Texpainter::FilterGraph::ParamName called_with_paramname{nullptr};
		Texpainter::FilterGraph::ParamValue called_with_paramvalue{0.0};
		proc->called_with_paramname  = &called_with_paramname;
		proc->called_with_paramvalue = &called_with_paramvalue;
		Texpainter::FilterGraph::Node obj{std: move(proc)};
		assert(obj.hasProcessor());
		obj.set("Param 1", Texpainter::FilterGraph::ParamValue{1.0});
		assert(called_with_paramname == "Param 1");
		assert(called_with_paramvalue == Texpainter::FilterGraph::ParamValue{1.0});
	}

	void texpainterFilterGraphNodeGetParam()
	{
		auto proc = std::make_unique<ImageProcessorStub>();
		Texpainter::FilterGraph::ParamName called_with_paramname{nullptr};
		proc->called_with_paramname = &called_with_paramname;
		Texpainter::FilterGraph::Node obj{std: move(proc)};
		assert(obj.hasProcessor());
		obj.get("Param 1");
		assert(called_with_paramname == "Param 1");
	}

	void texpainterFilterGraphNodeName()
	{
		Texpainter::FilterGraph::Node obj{std::make_unique<ImageProcessorStub>()};
		assert(obj.hasProcessor());
		assert(strcmp(obj.name(), "ImageProcessorStub") == 0);
	}
}

int main()
{
	Testcases::texpainterFilterGraphNodeDefaultState();
	Testcases::texpainterFilterGraphNodeDisconnectedCopy();
	Testcases::texpainterFilterGraphNodeReplaceWith();
	Testcases::texpainterFilterGraphNodeCall();
	Testcases::texpainterFilterGraphNodeInputPorts();
	Testcases::texpainterFilterGraphNodeOutputPorts();
	Testcases::texpainterFilterGraphNodeDisconnect();
	Testcases::texpainterFilterGraphNodeInputs();
	Testcases::texpainterFilterGraphNodeParamNames();
	Testcases::texpainterFilterGraphNodeParamValues();
	Testcases::texpainterFilterGraphNodeSetParam();
	Testcases::texpainterFilterGraphNodeGetParam();
	Testcases::texpainterFilterGraphNodeName();
	return 0;
}