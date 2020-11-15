//@	{
//@	  "targets":[{"name":"engine.hpp", "type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"engine.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_DFT_ENGINE_HPP
#define TEXPAINTER_DFT_ENGINE_HPP

#include "./plan.hpp"

#include <array>
#include <chrono>
#include <algorithm>

namespace Texpainter::Dft
{
	class Engine
	{
	public:
		template<Direction dir>
		void run(Size2d size,
		         BasicPlan::sample_type const* input_buffer,
		         BasicPlan::sample_type* output_buffer) const
		{
			auto& plan = get_plan(size);
			if constexpr(dir == Direction::Forward)
			{ plan.m_plan_fwd.execute(input_buffer, output_buffer); }
			else
			{
				plan.m_plan_bkwd.execute(input_buffer, output_buffer);
			}
		}

	private:
		struct PlanData
		{
			Size2d m_size{0, 0};
			std::chrono::time_point<std::chrono::steady_clock> m_last_used;
			Plan<Direction::Forward> m_plan_fwd{};
			Plan<Direction::Backward> m_plan_bkwd{};
		};

		mutable std::array<PlanData, 16> m_plans;

		PlanData const& get_plan(Size2d size) const
		{
			auto i = std::ranges::find_if(m_plans,
			                              [size](auto const& val) { return val.m_size == size; });
			if(i == std::end(m_plans)) [[unlikely]]
				{
					i         = std::ranges::min_element(m_plans, [](auto const& a, auto const& b) {
                        return a.m_last_used < b.m_last_used;
                    });
					i->m_size = size;
					i->m_plan_fwd  = Plan<Direction::Forward>{size};
					i->m_plan_bkwd = Plan<Direction::Backward>{size};
				}
			i->m_last_used = std::chrono::steady_clock::now();
			return *i;
		}
	};

	Engine const& engineInstance();
}

#endif