//@	{
//@	 "targets":[{"name":"image_processor_wrapper.test", "type":"application", "autorun":1}]
//@	}

#include "./image_processor_wrapper.hpp"

#include "utils/fixed_flatmap.hpp"

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
		};

		struct ParamNames
		{
			static constexpr auto items =
			    std::array<Texpainter::FilterGraph::ParamName, 3>{"Foo", "Bar", "Kaka"};
		};

		static constexpr char const* name() { return "Stub"; }
		static constexpr auto paramNames() { return ParamNames::items; }
		void set(std::string_view, Texpainter::FilterGraph::ParamValue) {}
		auto get(std::string_view) const { return Texpainter::FilterGraph::ParamValue{0.0f}; }
		auto paramValues() const { return std::span<Texpainter::FilterGraph::ParamValue const>{}; }

		auto operator()(std::span<Texpainter::FilterGraph::ImgProcArg const>) const
		{
			return std::vector<Texpainter::FilterGraph::ImgProcRetval>{};
		}

		void operator()(Texpainter::FilterGraph::ImgProcArg2<InterfaceDescriptor> const& args) const
		{
			args_result = &args;
		}

		mutable Texpainter::FilterGraph::ImgProcArg2<InterfaceDescriptor> const* args_result;


		struct ParamNameCompare
		{
			template<size_t n>
			constexpr bool operator()(Texpainter::FilterGraph::ParamName a,
			                          Texpainter::Str<n> const& b) const
			{
				return strcmp(a.c_str(), b.c_str()) < 0;
			}

			template<size_t n>
			constexpr bool operator()(Texpainter::Str<n> const& a,
			                          Texpainter::FilterGraph::ParamName b) const
			{
				return strcmp(a.c_str(), b.c_str()) < 0;
			}

			constexpr bool operator()(Texpainter::FilterGraph::ParamName a,
			                          Texpainter::FilterGraph::ParamName b) const
			{
				return a < b;
			}
		};

		Texpainter::FixedFlatmap<ParamNames, Texpainter::FilterGraph::ParamValue, ParamNameCompare>
		    m_params;

		auto getFoo() const { return m_params.find<Texpainter::Str("Foo")>(); }
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
		assert(std::equal(
		    std::begin(params), std::end(params), std::begin(ImgProcStub::paramNames())));
	}

	void texpainterFilterGraphImageProcessorWrapperParamValues()
	{
		Texpainter::FilterGraph::ImageProcessorWrapper obj{ImgProcStub{}};
		auto vals = obj.paramValues();
	}
#if 0

		ParamValue get(std::string_view param_name) const override
		{
			return m_proc.get(param_name);
		}

		AbstractImageProcessor& set(std::string_view param_name, ParamValue value) override
		{
			m_proc.set(param_name, value);
			return *this;
		}

		std::unique_ptr<AbstractImageProcessor> clone() const override
		{
			return std::make_unique<ImageProcessorWrapper>(*this);
		}

		char const* name() const override { return Proc::name(); }
#endif
}

int main()
{
	Testcases::texpaitnerFilterGraphImageProcessorWrapperCall();
	Testcases::texpaitnerFilterGraphImageProcessorWrapperInputPorts();
	Testcases::texpaitnerFilterGraphImageProcessorWrapperOutputPorts();
	Testcases::texpaitnerFilterGraphImageProcessorWrapperParamNames();
	Testcases::texpainterFilterGraphImageProcessorWrapperParamValues();


	return 0;
}