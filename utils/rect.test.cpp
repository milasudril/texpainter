//@	{"targets":[{"name":"rect.test", "type":"application", "autorun":1}]}

#include "./rect.hpp"

#include <cassert>
#include <cstdio>

namespace Testcases
{
	void texpainterRectAxisAlignBoundingBox()
	{
		Texpainter::vec2_t size_a{4.0, 3.0};
		auto const res_aa = Texpainter::axisAlignedBoundingBox(
		   size_a, Texpainter::Angle{atan(3.0 / 4.0), Texpainter::Angle::Radians{}});
		assert(res_aa[0] == 5);

		auto const res_ab = Texpainter::axisAlignedBoundingBox(
		   size_a, Texpainter::Angle{atan(4.0 / 3.0), Texpainter::Angle::Radians{}});
		assert(res_ab[1] == 5);

		Texpainter::vec2_t size_b{3.0, 4.0};
		auto const res_ba = Texpainter::axisAlignedBoundingBox(
		   size_b, Texpainter::Angle{atan(3.0 / 4.0), Texpainter::Angle::Radians{}});
		assert(res_ba[1] == 5);

		auto const res_bb = Texpainter::axisAlignedBoundingBox(
		   size_b, Texpainter::Angle{atan(4.0 / 3.0), Texpainter::Angle::Radians{}});
		assert(res_bb[0] == 5);
	}

	void texpainterRectAxisAlignBoundingBoxInvertedX()
	{
		Texpainter::vec2_t size_a{-4.0, 3.0};
		auto const res_aa = Texpainter::axisAlignedBoundingBox(
		   size_a, Texpainter::Angle{atan(3.0 / 4.0), Texpainter::Angle::Radians{}});
		assert(res_aa[0] == 5);
	}
}

int main()
{
	Testcases::texpainterRectAxisAlignBoundingBox();
	Testcases::texpainterRectAxisAlignBoundingBoxInvertedX();


	return 0;
}