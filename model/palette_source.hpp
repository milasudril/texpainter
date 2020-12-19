//@	{
//@	 "targets":[{"name":"palette_source.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_MODEL_PALETTESOURCE_HPP
#define TEXPAINTER_MODEL_PALETTESOURCE_HPP

#include "filtergraph/port_info.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"

#include "./palette.hpp"

#include <string>

namespace Texpainter::Model
{
	class PaletteSource
	{
	public:
		struct InterfaceDescriptor
		{
			static constexpr std::array<FilterGraph::PortInfo, 0> InputPorts{};
			static constexpr std::array<FilterGraph::PortInfo, 1> OutputPorts{
			    {{FilterGraph::PortType::Palette, "Palette"}}};
			static constexpr std::array<FilterGraph::ParamName, 0> ParamNames{};
		};

		explicit PaletteSource(std::string&& name): m_name{std::move(name)} {}

		void operator()(FilterGraph::ImgProcArg<InterfaceDescriptor> const& args) const
		{
			args.output<0>().get() = *r_pal;
		}

		FilterGraph::ParamValue get(FilterGraph::ParamName) const
		{
			return FilterGraph::ParamValue{0.0};
		}

		void set(FilterGraph::ParamName, FilterGraph::ParamValue) {}

		std::span<FilterGraph::ParamValue const> paramValues() const
		{
			return std::span<FilterGraph::ParamValue const>{};
		}

		char const* name() const { return m_name.c_str(); }

		static constexpr auto id() { return FilterGraph::InvalidImgProcId; }

		PaletteSource& source(std::reference_wrapper<Palette const> src)
		{
			r_pal = &src.get();
			return *this;
		}

	private:
		Palette const* r_pal;
		std::string m_name;
	};
}

#endif
