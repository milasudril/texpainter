//@	{"targets":[{"name":"span_2d.test", "type":"application", "autorun":1}]}

#include "./span_2d.hpp"

#include <cassert>

namespace Testcases
{
	void texpainterSpan2dBoundingBoxAllZeros()
	{
		constexpr std::array<float, 48> data{
		    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
		auto bb =
		    boundingBox(Texpainter::Span2d{data.data(), 8, 6}, [](auto val) { return val > 0.0f; });
		assert(!bb.valid());
	}

	void texpainterSpan2dBoundingBoxDiagUp()
	{
		// clang-format off
		constexpr std::array<float, 48> data{
		    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		    0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		    0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
		};
		// clang-format on

		auto bb =
		    boundingBox(Texpainter::Span2d{data.data(), 8, 6}, [](auto val) { return val > 0.0f; });

		assert(bb.valid());
		assert(bb.x_min == 1);
		assert(bb.x_max == 6);
		assert(bb.y_min == 1);
		assert(bb.y_max == 5);
		assert(bb.width() == 5);
		assert(bb.height() == 4);
	}

	void texpainterSpan2dBoundingBoxDiagDown()
	{
		// clang-format off
		constexpr std::array<float, 48> data{
		    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		    0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		    0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
		};
		// clang-format off

		auto bb =
		    boundingBox(Texpainter::Span2d{data.data(), 8, 6}, [](auto val) { return val > 0.0f; });

		assert(bb.valid());
		assert(bb.x_min == 1);
		assert(bb.x_max == 6);
		assert(bb.y_min == 1);
		assert(bb.y_max == 5);
		assert(bb.width() == 5);
		assert(bb.height() == 4);
	}
}

int main()
{
	Testcases::texpainterSpan2dBoundingBoxAllZeros();
	Testcases::texpainterSpan2dBoundingBoxDiagUp();
	Testcases::texpainterSpan2dBoundingBoxDiagDown();
	return 0;
}