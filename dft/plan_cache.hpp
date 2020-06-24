//@	{
//@	  "targets":[{"name":"plan_cache.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_DFT_PLANCACHE_HPP
#define TEXPAINTER_DFT_PLANCACHE_HPP

#include "./plan.hpp"

#include <array>

namespace Texpainter::Dft
{
	class PlanCache
	{
	public:
	private:
		struct PlanData
		{
			Size2d m_size;
			Plan<Direction::Forward> m_plan_fwd;
			Plan<direction::Backward> m_plan_bkwd;
			size_t m_use_count;
		};

		std::array<PlanData, 16> m_plans;
	};
}

#endif