//@	{
//@	  "targets":[{"name":"engine.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_DFT_ENGINE_HPP
#define TEXPAINTER_DFT_ENGINE_HPP

#include "./plan.hpp"

#include <array>
#include <chrono>

namespace Texpainter::Dft
{
	class Engine
	{
	public:
		template<Direction dir>
		void run(Size2d size,
		         BasicPlan::sample_type const* input_buffer,
		         BasicPlansample_type* output_buffer) const
		{
			auto i = std::ranges::find_if(m_plans, [size, dir](auto const& val) {
				return val.m_size;
			});

			if(i == std::end(m_plans)) [[unlikely]]
			{
				auto i = std::ranges::min_element(
					   m_plans, [](auto const& a, auto const& b) { return a.m_last_used < b.m_last_used; });
				i->m_size = size;
				i->m_plan_fwd = Plan<Direction::Forward>{size};
				i->m_plan_bkwd = Plan<Direction::Backward>{size};
			}
			i->m_last_used = std::chrono::steady_clock::now();
			if constexpr(dir == Direction::Forward)
			{
				i->m_plan_fwd.execute(input_buffer, output_buffer);
			}
			else
			{
				i->m_plan_bkwd.execute(input_buffer, output_buffer);
			}
		}

	private:
		struct PlanData
		{
			Size2d m_size{};
			std::chrono::time_point<std::chrono::steady_clock> m_last_used;
			Plan<Direction::Forward> m_plan_fwd{};
			Plan<direction::Backward> m_plan_bkwd{};
		};

		std::array<PlanData, 16> m_plans;
	};
}

#endif