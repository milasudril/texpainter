//@	{"targets":[{"name":"interpolation_table.test","type":"application", "autorun":1}]}

#include "./interpolation_table.hpp"

#include <cassert>

namespace Testcases
{
	void interpolationTableGetValues()
	{
		constexpr std::array<std::pair<double, double>, 3> vals{
		    {{0.0, 1.0}, {2.0, 2.0}, {4.0, 4.0}}};
		constexpr Texpainter::InterpolationTable tab{vals};

		assert(tab(-1.0) == 1.0);
		assert(tab(5.0) == 4.0);
		assert(tab(0.0) == 1.0);
		assert(tab(1.0) == 1.5);
		assert(tab(2.0) == 2.0);
		assert(tab(3.0) == 3.0);
		assert(tab(4.0) == 4.0);
	}
}

int main()
{
	Testcases::interpolationTableGetValues();
	return 0;
}