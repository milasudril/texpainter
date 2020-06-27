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

		CrackGenerator():
		   m_n_cracks{2},
		   m_line_width{1.0f / 128.0f},
		   m_seg_length{1.0 / 64.0},
		   m_branch_prob{0.25},
		   m_max_length{0.25}
		{
		}

		Model::Image operator()(Size2d output_size);

	private:
		Rng m_rng;
		int m_n_cracks;
		float m_line_width;
		double m_seg_length;
		double m_branch_prob;
		double m_max_length;
	};
}

#endif