//@	{
//@	 "targets":[{"name":"image_processor_wrapper.test", "type":"application", "autorun":1}]
//@	}

#include "./image_processor_wrapper.hpp"

#include "testutils/mallochook.hpp"

#include <cassert>

namespace
{
	struct ImgProcStub
	{
		struct InterfaceDescriptor
		{
			static constexpr auto InputPorts = std::array<Texpainter::FilterGraph::PortInfo, 3>{
			    {{Texpainter::FilterGraph::PixelType::RGBA, "Input 1"},
			     {Texpainter::FilterGraph::PixelType::GrayscaleReal, "Input 2"},
			     {Texpainter::FilterGraph::PixelType::GrayscaleComplex, "Input 3"}}};

			static constexpr auto OutputPorts = std::array<Texpainter::FilterGraph::PortInfo, 4>{
			    {{Texpainter::FilterGraph::PixelType::GrayscaleComplex, "Output 1"},
			     {Texpainter::FilterGraph::PixelType::GrayscaleReal, "Output 2"},
			     {Texpainter::FilterGraph::PixelType::RGBA, "Output 3"},
			     {Texpainter::FilterGraph::PixelType::GrayscaleReal, "Output 4"}}};

			static constexpr auto ParamNames =
			    std::array<Texpainter::FilterGraph::ParamName, 3>{"Foo", "Bar", "Kaka"};
		};

		static constexpr char const* name() { return "Stub"; }

		void set(Texpainter::FilterGraph::ParamName name, Texpainter::FilterGraph::ParamValue val)
		{
			param_name_asked_for = &name;
			param_value_set = val;
		}

		auto get(Texpainter::FilterGraph::ParamName name) const
		{
			param_name_asked_for = &name;
			return Texpainter::FilterGraph::ParamValue{0};
		}

		auto const& paramValues() const { return m_params; }

		auto operator()(std::span<Texpainter::FilterGraph::ImgProcArg const>) const
		{
			return std::vector<Texpainter::FilterGraph::ImgProcRetval>{};
		}

		void operator()(Texpainter::FilterGraph::ImgProcArg2<InterfaceDescriptor> const& args) const
		{
			args_result = &args;
		}

		std::array<Texpainter::FilterGraph::ParamValue, 3> m_params{
		    Texpainter::FilterGraph::ParamValue{1},
		    Texpainter::FilterGraph::ParamValue{2},
		    Texpainter::FilterGraph::ParamValue{3}};


		mutable Texpainter::FilterGraph::ImgProcArg2<InterfaceDescriptor> const* args_result{nullptr};
		mutable Texpainter::FilterGraph::ParamName* param_name_asked_for{nullptr};
		mutable Texpainter::FilterGraph::ParamValue param_value_set{0};
	};
}

namespace Testcases
{
	void texpaitnerFilterGraphImageProcessorWrapperCall()
	{
		Texpainter::FilterGraph::ImageProcessorWrapper obj{ImgProcStub{}};

		Texpainter::Size2d size{3, 2};

		std::array<Texpainter::FilterGraph::RgbaValue, 6> input1{};
		std::array<Texpainter::FilterGraph::RealValue, 6> input2{};
		std::array<Texpainter::FilterGraph::ComplexValue, 6> input3{};

		Texpainter::FilterGraph::NodeArgument na{
		    size, {{std::begin(input1), std::begin(input2), std::begin(input3)}}};

		Testutils::MallocHook::init();
		auto ret = obj(na);
		Testutils::MallocHook::disarm();
		auto args = *obj.processor().args_result;

		assert(args.size() == size);
		assert(ret[0].get() == args.output<0>());
		assert(ret[1].get() == args.output<1>());
		assert(ret[2].get() == args.output<2>());
		assert(ret[3].get() == args.output<3>());
		assert(Testutils::MallocHook::blockSize(args.output<0>())
		       == size.area() * sizeof(Texpainter::FilterGraph::ComplexValue));
		assert(Testutils::MallocHook::blockSize(args.output<1>())
		       == size.area() * sizeof(Texpainter::FilterGraph::RealValue));
		assert(Testutils::MallocHook::blockSize(args.output<2>())
		       == size.area() * sizeof(Texpainter::FilterGraph::RgbaValue));
		assert(Testutils::MallocHook::blockSize(args.output<3>())
		       == size.area() * sizeof(Texpainter::FilterGraph::RealValue));
		static_assert(
		    std::is_same_v<decltype(args.output<0>()), Texpainter::FilterGraph::ComplexValue*>);
		static_assert(
		    std::is_same_v<decltype(args.output<1>()), Texpainter::FilterGraph::RealValue*>);
		static_assert(
		    std::is_same_v<decltype(args.output<2>()), Texpainter::FilterGraph::RgbaValue*>);
		static_assert(
		    std::is_same_v<decltype(args.output<3>()), Texpainter::FilterGraph::RealValue*>);

		assert(std::begin(input1) == args.input<0>());
		assert(std::begin(input2) == args.input<1>());
		assert(std::begin(input3) == args.input<2>());
		static_assert(
		    std::is_same_v<decltype(args.input<0>()), Texpainter::FilterGraph::RgbaValue const*>);
		static_assert(
		    std::is_same_v<decltype(args.input<1>()), Texpainter::FilterGraph::RealValue const*>);
		static_assert(std::is_same_v<decltype(args.input<2>()),
		                             Texpainter::FilterGraph::ComplexValue const*>);
	}

	void texpaitnerFilterGraphImageProcessorWrapperInputPorts()
	{
		Texpainter::FilterGraph::ImageProcessorWrapper obj{ImgProcStub{}};

		auto ports = obj.inputPorts();
		assert(std::ranges::equal(ports, ImgProcStub::InterfaceDescriptor::InputPorts));
	}

	void texpaitnerFilterGraphImageProcessorWrapperOutputPorts()
	{
		Texpainter::FilterGraph::ImageProcessorWrapper obj{ImgProcStub{}};

		auto ports = obj.outputPorts();
		assert(std::ranges::equal(ports, ImgProcStub::InterfaceDescriptor::OutputPorts));
	}

	void texpaitnerFilterGraphImageProcessorWrapperParamNames()
	{
		Texpainter::FilterGraph::ImageProcessorWrapper obj{ImgProcStub{}};

		auto params = obj.paramNames();
		assert(std::ranges::equal(params, ImgProcStub::InterfaceDescriptor::ParamNames));
	}

	void texpainterFilterGraphImageProcessorWrapperParamValues()
	{
		Texpainter::FilterGraph::ImageProcessorWrapper obj{ImgProcStub{}};
		auto vals = obj.paramValues();
		assert(std::ranges::equal(vals, obj.processor().m_params));
	}

	void texpainterFilterGraphImageProcessorWrapperGetParamValue()
	{
		Texpainter::FilterGraph::ImageProcessorWrapper obj{ImgProcStub{}};
		(void)obj.get(Texpainter::FilterGraph::ParamName{"Hej"});
		auto asked_for = *obj.processor().param_name_asked_for;
		assert((asked_for == Texpainter::FilterGraph::ParamName{"Hej"}));
	}

	void texpainterFilterGraphImageProcessorWrapperSetParamValue()
	{
		Texpainter::FilterGraph::ImageProcessorWrapper obj{ImgProcStub{}};
		obj.set(Texpainter::FilterGraph::ParamName{"Hej"}, Texpainter::FilterGraph::ParamValue{128});
		auto asked_for = *obj.processor().param_name_asked_for;
		auto value_set = obj.processor().param_value_set;
		assert((asked_for == Texpainter::FilterGraph::ParamName{"Hej"}));
		assert((value_set == Texpainter::FilterGraph::ParamValue{128}));
	}

	void texpainterFilterGraphImageProcessorWrapperName()
	{
		Texpainter::FilterGraph::ImageProcessorWrapper obj{ImgProcStub{}};
		assert(strcmp(obj.name(), ImgProcStub::name()) == 0);
	}
}

int main()
{
	Testcases::texpaitnerFilterGraphImageProcessorWrapperCall();
	Testcases::texpaitnerFilterGraphImageProcessorWrapperInputPorts();
	Testcases::texpaitnerFilterGraphImageProcessorWrapperOutputPorts();
	Testcases::texpaitnerFilterGraphImageProcessorWrapperParamNames();
	Testcases::texpainterFilterGraphImageProcessorWrapperParamValues();
	Testcases::texpainterFilterGraphImageProcessorWrapperGetParamValue();
	Testcases::texpainterFilterGraphImageProcessorWrapperSetParamValue();
	Testcases::texpainterFilterGraphImageProcessorWrapperName();

	return 0;
}