//@	{
//@	 "targets":[{"name":"node_argument.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_NODEARGUMENT_HPP
#define TEXPAINTER_FILTERGRAPH_NODEARGUMENT_HPP

#include "./port_value.hpp"

#include "utils/size_2d.hpp"

#include <array>
#include <functional>

namespace Texpainter::FilterGraph
{
	class NodeArgument
	{
	public:
		static constexpr size_t MaxNumInputs = 4;
		explicit NodeArgument(Size2d size,
		                      float resolution,
		                      uint64_t rng_seed,
		                      std::array<InputPortValue, MaxNumInputs> const& inputs)
		    : m_size{size}
		    , m_resolution{resolution}
		    , m_rng_seed{rng_seed}
		    , r_inputs{inputs}
		{
		}

		auto const& inputs() const { return r_inputs; }

		Size2d size() const { return m_size; }

		float resolution() const { return m_resolution; }

		uint64_t rngSeed() const { return m_rng_seed ;}

	private:
		Size2d m_size;
		float m_resolution;
		uint64_t m_rng_seed;
		std::array<InputPortValue, MaxNumInputs> r_inputs;
	};
}

#endif