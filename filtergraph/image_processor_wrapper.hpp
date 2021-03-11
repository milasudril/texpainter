//@	{
//@	 "targets":[{"name":"image_processor_wrapper.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_IMAGEPROCESSORWRAPPER_HPP
#define TEXPAINTER_FILTERGRAPH_IMAGEPROCESSORWRAPPER_HPP

#include "./node_argument.hpp"
#include "./port_info.hpp"
#include "./img_proc_arg.hpp"
#include "./abstract_image_processor.hpp"
#include "./image_processor.hpp"

namespace Texpainter::FilterGraph
{
	namespace detail
	{
		template<class Buffers, size_t n = Buffers::size()>
		void take_output_buffers(Buffers& buffers, std::array<PortValue, 4>& ret)
		{
			if constexpr(n != 0)
			{
				ret[n - 1] = std::move(buffers.template take<n - 1>());
				take_output_buffers<Buffers, n - 1>(buffers, ret);
			}
		}
	}

	template<ImageProcessor Proc>
	class ImageProcessorWrapper final: public AbstractImageProcessor
	{
	public:
		explicit ImageProcessorWrapper(Proc&& proc): m_proc{std::move(proc)} {}

		result_type operator()(NodeArgument const& arg) const override
		{
			using InterfaceDescriptor = typename Proc::InterfaceDescriptor;

			using InputArgs  = typename ImgProcArg<InterfaceDescriptor>::InputArgs;
			using OutputArgs = typename ImgProcArg<InterfaceDescriptor>::OutputArgs;

			OutputBuffers<portTypes(InterfaceDescriptor::OutputPorts)> outputs{arg.size()};

			m_proc(ImgProcArg<InterfaceDescriptor>{
			    arg.size(),
			    arg.resolution(),
			    createTuple<InputArgs>([&inputs = arg.inputs()]<class Tag>(Tag) {
				    constexpr auto types = portTypes(InterfaceDescriptor::InputPorts);
				    using InputT  = typename detail::GenInputPortType<types[Tag::value]>::type;
				    using OutputT = std::tuple_element_t<Tag::value, InputArgs>;
				    static_assert(std::is_same_v<InputT, OutputT>);
				    return *Enum::get_if<InputT>(&inputs[Tag::value]);
			    }),
			    createTuple<OutputArgs>(
			        [&outputs]<class T>(T) { return outputs.template get<T::value>(); })});

			result_type ret;
			detail::take_output_buffers(outputs, ret);
			return ret;
		}

		std::span<PortInfo const> inputPorts() const override
		{
			return Proc::InterfaceDescriptor::InputPorts;
		}

		std::span<PortInfo const> outputPorts() const override
		{
			return Proc::InterfaceDescriptor::OutputPorts;
		}

		std::span<ParamName const> paramNames() const override
		{
			return Proc::InterfaceDescriptor::ParamNames;
		}

		ParamValue get(ParamName param_name) const override { return m_proc.get(param_name); }

		AbstractImageProcessor& set(ParamName param_name, ParamValue value) override
		{
			m_proc.set(param_name, value);
			return *this;
		}

		std::unique_ptr<AbstractImageProcessor> clone() const override
		{
			return std::make_unique<ImageProcessorWrapper>(*this);
		}

		char const* name() const override { return m_proc.name(); }

		ImageProcessorId id() const override { return Proc::id(); }

		Proc const& processor() const { return m_proc; }

		Proc& processor() { return m_proc; }

		ImgProcReleaseState releaseState() const { return Proc::releaseState(); }

	private:
		Proc m_proc;
	};

	template<ImageProcessor Proc>
	std::unique_ptr<AbstractImageProcessor> createImageProcessor()
	{
		return std::make_unique<ImageProcessorWrapper<Proc>>(Proc{});
	}
}

#endif