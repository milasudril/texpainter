//@	{
//@	 "targets":[{"name":"image_source.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_IMAGESOURCE_HPP
#define TEXPAINTER_FILTERGRAPH_IMAGESOURCE_HPP

#include "./proctypes.hpp"
#include "./img_proc_arg.hpp"

namespace Texpainter::FilterGraph
{
	namespace detail
	{
		template<class T>
		struct MapPixelType;

		template<>
		struct MapPixelType<PixelStore::Pixel>
		{
			static constexpr auto value = PixelType::RGBA;
		};
	}

	template<class PixelType>
	class ImageSource
	{
	public:
		struct InterfaceDescriptor
		{
			static constexpr std::array<PortInfo, 0> InputPorts{};
			static constexpr std::array<PortInfo, 1> OutputPorts{
			    {{typeToPixelType<PixelType>(), "Pixels"}}};
			static constexpr std::array<ParamName, 0> ParamNames{};
		};

		void operator()(ImgProcArg2<InterfaceDescriptor> const& args) const
		{
			std::ranges::copy(r_pixels, args.template output<0>());
		}

		ParamValue get(ParamName) const { return ParamValue{0.0}; }

		void set(ParamName, ParamValue) {}

		std::span<ParamValue const> paramValues() const { return std::span<ParamValue const>{}; }

		static constexpr char const* name() { return "Layer input"; }

		ImageSource& source(Span2d<PixelType const> src)
		{
			r_pixels = src;
			return *this;
		}

	private:
		Span2d<PixelType const> r_pixels;
	};
}

#endif
