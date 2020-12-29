//@	{"targets":[{"name":"bidirectional_interpolation_table.test","type":"application", "autorun":1}]}

#include "./bidirectional_interpolation_table.hpp"

#include <cassert>

namespace Testcases
{
	void bidirectionalInterpolationTableGetValues()
	{
		constexpr std::array<std::pair<double, double>, 3> vals{
		    {{0.0, 1.0}, {2.0, 2.0}, {4.0, 4.0}}};
		constexpr Texpainter::BidirectionalInterpolationTable tab{vals};

		assert(tab.output(-1.0) == 1.0);
		assert(tab.output(5.0) == 4.0);
		assert(tab.output(0.0) == 1.0);
		assert(tab.output(1.0) == 1.5);
		assert(tab.output(2.0) == 2.0);
		assert(tab.output(3.0) == 3.0);
		assert(tab.output(4.0) == 4.0);

		assert(tab.input(0.0) == 0.0);
		assert(tab.input(5.0) == 4.0);
		assert(tab.input(1.0) == 0.0);
		assert(tab.input(1.5) == 1.0);
		assert(tab.input(2.0) == 2.0);
		assert(tab.input(3.0) == 3.0);
		assert(tab.input(4.0) == 4.0);

		assert(tab.input(tab.output(0.0)) == 0.0);
		assert(tab.input(tab.output(1.0)) == 1.0);
		assert(tab.input(tab.output(2.0)) == 2.0);
		assert(tab.input(tab.output(3.0)) == 3.0);
		assert(tab.input(tab.output(4.0)) == 4.0);

		assert(tab.output(tab.input(1.0)) == 1.0);
		assert(tab.output(tab.input(1.5)) == 1.5);
		assert(tab.output(tab.input(2.0)) == 2.0);
		assert(tab.output(tab.input(3.0)) == 3.0);
		assert(tab.output(tab.input(4.0)) == 4.0);
	}

	void bidirectionalInterpolationTableGetValues2()
	{
		constexpr std::array<std::pair<float, float>, 9> vals{{{0.0f, 0.0f},
		                                                       {0.125f, 3.6586624e-02f},
		                                                       {0.25f, 1.2826073e-01f},
		                                                       {0.375f, 2.4137302e-01f},
		                                                       {0.5f, 3.3333334e-01f},
		                                                       {0.625f, 0.416f},
		                                                       {0.75f, 0.625f},
		                                                       {0.875f, 0.8125f},
		                                                       {1.0f, 1.0f}}};

		constexpr Texpainter::BidirectionalInterpolationTable tab{vals};

		assert(tab.input(3.3333334e-01f) == 0.5f);
	}
}

int main()
{
	Testcases::bidirectionalInterpolationTableGetValues();
	Testcases::bidirectionalInterpolationTableGetValues2();
	return 0;
}