//@	{
//@	 "targets":[{"name":"node.test", "type":"application", "autorun":1}]
//@	}

#include "./node.hpp"
#include <cassert>

namespace
{
	class ImageProcessorStub final: public Texpainter::FilterGraph::AbstractImageProcessor
	{
	public:
		static constexpr std::array<Texpainter::FilterGraph::PortInfo const, 2> s_input_ports{
		    {{Texpainter::FilterGraph::PortType::RgbaPixels, "Input 1"},
		     {Texpainter::FilterGraph::PortType::GrayscaleRealPixels, "Input 2"}}};

		static constexpr std::array<Texpainter::FilterGraph::PortInfo const, 3> s_output_ports{
		    {{Texpainter::FilterGraph::PortType::RgbaPixels, "Output 1"},
		     {Texpainter::FilterGraph::PortType::GrayscaleComplexPixels, "Output 2"},
		     {Texpainter::FilterGraph::PortType::GrayscaleRealPixels, "Output 3"}}};

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
		using result_type = Texpainter::FilterGraph::AbstractImageProcessor::result_type;
		result_type operator()(Texpainter::FilterGraph::NodeArgument const&) const override
		{
			return result_type{std::make_unique<Texpainter::FilterGraph::RgbaValue[]>(1),
			                   std::make_unique<Texpainter::FilterGraph::ComplexValue[]>(1),
			                   std::make_unique<Texpainter::FilterGraph::RealValue[]>(1)};
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

		Texpainter::FilterGraph::ImageProcessorId id() const override
		{
			return Texpainter::FilterGraph::ImageProcessorId{"32520701CBF9985E5E3866051A9FC52D"};
		}
	};
}

namespace Testcases
{
	void texpainterFilterGraphNodeDefaultState()
	{
		Texpainter::FilterGraph::Node obj;
		assert(!obj.hasProcessor());
		assert(!obj.isConnected(Texpainter::FilterGraph::InputPortIndex{0}));
		assert(!obj.isConnected(Texpainter::FilterGraph::InputPortIndex{1}));
		assert(!obj.isConnected(Texpainter::FilterGraph::InputPortIndex{2}));
		assert(!obj.isConnected(Texpainter::FilterGraph::InputPortIndex{3}));
		assert(!isConnected(obj));
	}

	void texpainterFilterGraphNodeClonedProcessor()
	{
		Texpainter::FilterGraph::Node obj{std::make_unique<ImageProcessorStub>()};
		assert(obj.hasProcessor());
		auto other = obj.clonedProcessor();
		assert(other.get() != &obj.processor());
	}

	void texpainterFilterGraphNodeCall()
	{
		Texpainter::FilterGraph::Node obj{std::make_unique<ImageProcessorStub>()};
		assert(obj.hasProcessor());

		ImageProcessorStub input_stub;
		input_stub.no_inputs = true;
		Texpainter::FilterGraph::Node input{std::make_unique<ImageProcessorStub>(input_stub)};

		obj.connect(Texpainter::FilterGraph::InputPortIndex{0},
		            input,
		            Texpainter::FilterGraph::OutputPortIndex{0})
		    .connect(Texpainter::FilterGraph::InputPortIndex{1},
		             input,
		             Texpainter::FilterGraph::OutputPortIndex{2});
		obj(Texpainter::Size2d{1, 1}, 1.0);
	}

	void texpainterFilterGraphNodeInputPortIndexs()
	{
		Texpainter::FilterGraph::Node obj{std::make_unique<ImageProcessorStub>()};
		assert(obj.hasProcessor());
		auto ports = obj.inputPorts();
		assert(std::ranges::equal(ports, ImageProcessorStub::s_input_ports));
	}

	void texpainterFilterGraphNodeOutputPortIndexs()
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

		obj.connect(Texpainter::FilterGraph::InputPortIndex{0},
		            input,
		            Texpainter::FilterGraph::OutputPortIndex{0})
		    .connect(Texpainter::FilterGraph::InputPortIndex{1},
		             input,
		             Texpainter::FilterGraph::OutputPortIndex{2});
		assert(isConnected(obj));

		obj.disconnect(Texpainter::FilterGraph::InputPortIndex{0});
		assert(!obj.isConnected(Texpainter::FilterGraph::InputPortIndex{0}));
		assert(obj.isConnected(Texpainter::FilterGraph::InputPortIndex{1}));
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

		obj.connect(Texpainter::FilterGraph::InputPortIndex{0},
		            input,
		            Texpainter::FilterGraph::OutputPortIndex{0})
		    .connect(Texpainter::FilterGraph::InputPortIndex{1},
		             input,
		             Texpainter::FilterGraph::OutputPortIndex{2});

		assert((std::ranges::all_of(obj.inputs(), [](auto item) { return item.valid(); })));
		std::array<Texpainter::FilterGraph::Node const*, 2> nodes{&input, &input};
		assert((std::ranges::equal(obj.inputs(), nodes, [](auto const& item_a, auto const& item_b) {
			return &item_a.processor() == item_b;
		})));

		std::array<Texpainter::FilterGraph::OutputPortIndex, 2> output_ports{
		    Texpainter::FilterGraph::OutputPortIndex{0},
		    Texpainter::FilterGraph::OutputPortIndex{2}};
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
	Testcases::texpainterFilterGraphNodeClonedProcessor();
	Testcases::texpainterFilterGraphNodeCall();
	Testcases::texpainterFilterGraphNodeInputPortIndexs();
	Testcases::texpainterFilterGraphNodeOutputPortIndexs();
	Testcases::texpainterFilterGraphNodeDisconnect();
	Testcases::texpainterFilterGraphNodeInputs();
	Testcases::texpainterFilterGraphNodeParamNames();
	Testcases::texpainterFilterGraphNodeParamValues();
	Testcases::texpainterFilterGraphNodeSetParam();
	Testcases::texpainterFilterGraphNodeGetParam();
	Testcases::texpainterFilterGraphNodeName();
	return 0;
}