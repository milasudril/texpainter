//@	{
//@	 "targets":[{"name":"img_proc_arg.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_IMGPROCARG_HPP
#define TEXPAINTER_FILTERGRAPH_IMGPROCARG_HPP

#include "./argtuple.hpp"
#include "./img_proc_interface_descriptor.hpp"
#include "./utils/default_rng.hpp"

#include "utils/size_2d.hpp"

#include <functional>

namespace Texpainter::FilterGraph
{
	template<ImgProcInterfaceDescriptor Descriptor>
	class ImgProcArg
	{
	public:
		using InputArgs  = InArgTuple<portTypes(Descriptor::InputPorts)>;
		using OutputArgs = OutArgTuple<portTypes(Descriptor::OutputPorts)>;

		ImgProcArg(): m_canvas_size{0, 0} {}

		explicit ImgProcArg(Size2d canvas_size,
		                    float resolution,
		                    DefaultRng::SeedValue rng_seed,
		                    InputArgs const& inputs,
		                    OutputArgs const& outputs)
		    : m_canvas_size{canvas_size}
		    , m_resolution{resolution}
		    , m_rng_seed{rng_seed}
		    , m_inputs{inputs}
		    , m_outputs{outputs}
		{
		}

		Size2d canvasSize() const { return m_canvas_size; }

		template<size_t index>
		auto input(uint32_t col, uint32_t row) const
		{
			return std::get<index>(m_inputs)[row * m_canvas_size.width() + col];
		}

		template<size_t index>
		auto input() const
		{
			return std::get<index>(m_inputs);
		}

		template<size_t index>
		auto& output(uint32_t col, uint32_t row) const
		{
			return std::get<index>(m_outputs)[row * m_canvas_size.width() + col];
		}

		template<size_t index>
		auto output() const
		{
			return std::get<index>(m_outputs);
		}

		auto resolution() const { return m_resolution; }

		auto rngSeed() const { return m_rng_seed; }

	private:
		Size2d m_canvas_size;
		float m_resolution;
		DefaultRng::SeedValue m_rng_seed;
		InputArgs m_inputs;
		OutputArgs m_outputs;
	};

	template<size_t index, ImgProcInterfaceDescriptor Descriptor>
	auto input(ImgProcArg<Descriptor> const& d, uint32_t col, uint32_t row)
	{
		return d.template input<index>(col, row);
	}

	template<size_t index, ImgProcInterfaceDescriptor Descriptor>
	auto input(ImgProcArg<Descriptor> const& d)
	{
		return d.template input<index>();
	}

	template<size_t index, ImgProcInterfaceDescriptor Descriptor>
	auto& output(ImgProcArg<Descriptor> const& d, uint32_t col, uint32_t row)
	{
		return d.template output<index>(col, row);
	}

	template<size_t index, ImgProcInterfaceDescriptor Descriptor>
	auto output(ImgProcArg<Descriptor> const& d)
	{
		return d.template output<index>();
	}
}

#endif