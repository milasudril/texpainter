//@	{
//@	 "targets":[{"name":"param_map.hpp", "type":"include"}]
//@	}


#ifndef TEXPAINTER_FILTERGRAPH_PARAMMAP_HPP
#define TEXPAINTER_FILTERGRAPH_PARAMMAP_HPP

#include "./img_proc_param.hpp"

#include "utils/fixed_flatmap.hpp"

namespace Texpainter::FilterGraph
{
	namespace detail
	{
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

		template<class InterfaceDescriptor>
		struct ParamNames
		{
			static constexpr auto items = InterfaceDescriptor::ParamNames;
		};
	}

	template<class InterfaceDescriptor>
	using ParamMap = Texpainter::
	    FixedFlatmap<detail::ParamNames<InterfaceDescriptor>, ParamValue, detail::ParamNameCompare>;
}

#endif