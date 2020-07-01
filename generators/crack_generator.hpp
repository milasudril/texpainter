//@	{
//@	  "targets":[{"name":"crack_generator.hpp", "type":"include"}]
//@	 ,"dependencies_extra":[{"ref": "crack_generator.o", "rel":"implementation"}]
//@	 }

#ifndef TEXPAINTER_CRACKGENERATOR_HPP
#define TEXPAINTER_CRACKGENERATOR_HPP

#include "model/image.hpp"

#include <random>

namespace Texpainter::Generators
{
	class CrackGenerator
	{
	public:
		using Rng = std::mt19937;

		CrackGenerator(Rng& rng):
		   r_rng{rng},
		   m_n_cracks{3},
		   m_line_width{1.0f / 64.0f},
		   m_line_width_growth{0.9f},
		   m_noise_mod{0.5f},
		   m_seg_length{1.0 / 64.0},
		   m_branch_prob{0.125},
		   m_max_length{0.5}
		{
		}

		Model::Image operator()(Size2d output_size);

		int nCracks() const
		{
			return m_n_cracks;
		}

		CrackGenerator nCracks(int value)
		{
			m_n_cracks = value;
			return *this;
		}

		float lineWidth() const
		{
			return m_line_width;
		}

		CrackGenerator& lineWidth(float value)
		{
			m_line_width = value;
			return *this;
		}

		float lineWidthGrowth() const
		{
			return m_line_width_growth;
		}

		CrackGenerator& lineWidthGrowth(float value)
		{
			m_line_width_growth = value;
			return *this;
		}

		double segLength() const
		{
			return m_seg_length;
		}

		CrackGenerator& segLength(double value)
		{
			m_seg_length = value;
			return *this;
		}

		double branchProb() const
		{
			return m_branch_prob;
		}

		CrackGenerator& branchProb(double val)
		{
			m_branch_prob = val;
			return *this;
		}

		double maxLength() const
		{
			return m_max_length;
		}

		CrackGenerator& maxLength(double value)
		{
			m_max_length = value;
			return *this;
		}

		float noiseMod() const
		{
			return m_noise_mod;
		}

		CrackGenerator& noiseMod(float val)
		{
			m_noise_mod = val;
			return *this;
		}

	private:
		Rng& r_rng;
		int m_n_cracks;
		float m_line_width;
		float m_line_width_growth;
		float m_noise_mod;
		double m_seg_length;
		double m_branch_prob;
		double m_max_length;
	};
}

#endif